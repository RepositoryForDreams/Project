#pragma once
#include <Common/Define.h>
#include <atomic>
#include <typeinfo>


#define JGTYPE(x) Type(TypeID<##x>())
#define TYPE_NULL_ID -1
namespace JG
{
	template<class T>
	class TypeID 
	{
		friend class Type;
		static u64 ID;
	public:
		TypeID(T* _this = nullptr)
		{
			if (ID == TYPE_NULL_ID)
			{
				ID = typeid(T).hash_code();
			}
		}
	};
	template<class T>
	u64 TypeID<T>::ID = TYPE_NULL_ID;



	class Type
	{
		u64    mID = TYPE_NULL_ID;
		String mName;
	public:
		Type()
		{
			mID   = TYPE_NULL_ID;
			mName = TT("NULL");
		}

		template<class T>
		constexpr Type(const TypeID<T>& typeID)
		{
			mID = typeID.ID;
			mName = s2ws(typeid(T).name());
			u64 pos = mName.find(TT(" "), 0);
			if (pos != String::npos)
			{
				mName = mName.substr(pos + 1);
			}
			pos = mName.find_last_of(TT("::"));
			if (pos != String::npos)
			{
				mName = mName.substr(pos + 1);
			}
		}
		

		template<class T>
		constexpr Type& operator=(const TypeID<T>& typeID)
		{
			mID = typeID.ID;
			mName = s2ws(typeid(T).name());
			u64 pos = mName.find(TT(" "), 0);
			if (pos != String::npos)
			{
				mName = mName.substr(pos + 1);
			}
			pos = mName.find_last_of(TT("::"));
			if (pos != String::npos)
			{
				mName = mName.substr(pos + 1);
			}
			return *this;
		}
		Type& operator=(const Type& type)
		{
			mID   = type.mID;
			mName = type.mName;
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