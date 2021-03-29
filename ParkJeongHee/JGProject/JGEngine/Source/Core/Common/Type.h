#pragma once
#include <Common/Define.h>
#include <atomic>
#include <typeinfo>




namespace JG
{
	class TypeIDProvider
	{
	protected:
		template<class T>
		friend class TypeID;


		const static u64 TYPE_NULL_ID = -1;
		static std::atomic<u64> IDOffset;
	private:
		TypeIDProvider() = default;
	};




	template<class T>
	class TypeID 
	{
		friend class Type;
		static u64 ID;
	public:
		TypeID()
		{
			if (ID == TypeIDProvider::TYPE_NULL_ID)
			{
				ID = TypeIDProvider::IDOffset++;
			}
		}
	};
	template<class T>
	u64 TypeID<T>::ID = TypeIDProvider::TYPE_NULL_ID;



	class Type
	{
		u64    mID = 0;
		String mName;
	public:
		template<class T>
		constexpr Type(const TypeID<T>& typeID)
		{
			mID = typeID.ID;
			mName = s2ws(typeid(T).name());
		}
		template<class T>
		constexpr Type& operator=(const TypeID<T>& typeID)
		{
			mID = typeID.ID;
			mName = s2ws(typeid(T).name());
			return *this;
		}
		Type& operator=(const Type& type)
		{
			mID = type.mID;
			return *this;
		}
		bool operator==(const Type& type) const
		{
			return mID == type.mID;
		}
		bool operator!=(const Type& type) const
		{
			return mID != type.mID;
		}
	public:
		u64 GetID() const {
			return mID;
		}
		const String& GetName() const {
			return mName;
		}
	private:

		friend std::hash<JG::Type>;
	};



	


}


namespace std
{
	template<>
	struct hash<JG::Type>
	{
		size_t operator()(JG::Type const& type) const noexcept
		{
			return type.mID;
		}
	};
}