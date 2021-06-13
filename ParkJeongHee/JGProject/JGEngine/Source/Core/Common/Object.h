#pragma once
#include "Define.h"
#include "Type.h"
#include "Class/FileIO.h"
namespace JG
{

#define JGCLASS \
public: \
	virtual Type GetType() const override { return Type(TypeID(this));} \
private: \


	class IInspectorGUI
	{
	public:
		virtual void OnInspectorGUI() = 0;
	};

	class ISubscriber
	{
	public:
		virtual ~ISubscriber() = default;
	};



	

	class IJGObject : public IInspectorGUI, public ISubscriber, public ISerializable
	{
	protected:
		struct JGMetaData
		{
			String Type;
			List<jbyte>  Data;
		};
		using JGMetaDataMap = Dictionary<String, JGMetaData>;
	private:
		Dictionary<String, JGMetaData> mMetaDatas;
	public:
		virtual Type GetType() const  = 0;
		virtual const String& GetName() const 
		{
			return GetType().GetName();
		}
	private:
		virtual void Serialize(FileStreamWriter* writer)   const override
		{
			Serialize();
			writer->Write(mMetaDatas.size());
			for (auto& _pair : mMetaDatas)
			{
				writer->Write(_pair.first);
				writer->Write(_pair.second.Type);
				writer->Write(_pair.second.Data);
			}
		}
		virtual void DeSerialize(FileStreamReader* reader) override
		{
			u64 size = 0;
			reader->Read(&size);

			for (u64 i = 0; i < size; ++i)
			{
				JGMetaData meta;
				String name;
				reader->Read(&name);
				reader->Read(&(meta.Type));
				reader->Read(&(meta.Data));
				mMetaDatas.emplace(name, meta);
			}
			DeSerialize();
			mMetaDatas.clear();
		}
	protected:
		template<class T>
		void SerializeVar(const String& key, T& data) const
		{
			MetaData meta;
			meta.Type = JGTYPE(T).GetName();
			meta.Data.resize(sizeof(T));
			memcpy(&meta.Data[0], &data, sizeof(T));
			mMetaDatas.emplace(key, meta);
		}
		template<class T>
		bool DeSerializeVar(const String& key, T* pData)
		{
			auto iter = mMetaDatas.find(key);
			if (iter == mMetaDatas.find(key).end())
			{
				return false;
			}

			if (iter->second.Type != JGTYPE(T).GetName())
			{
				return false;
			}

			memcpy(pData, iter->second.Data.data(), iter->second.Data.size());
			return true;
		}


		virtual void Serialize() const {};
		virtual void DeSerialize() {}
	public:
		virtual void OnInspectorGUI() override {}
		virtual ~IJGObject() = default;
	};
}