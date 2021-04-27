#pragma once
#include "Common/Abstract.h"
#include "Common/Type.h"
#include "Graphics/Resource.h"
#include "Class/Global/Scheduler.h"
#include "Class/FileIO.h"
namespace JG
{
#define ASSET_MESH_FORMAT TT(".mesh")

	struct JGVertex
	{
		JVector3 Position;
		JVector3 Normal;
		JVector3 Tangent;
		JVector3 Bitangent;
		JVector2 Texcoord;

		static SharedPtr<InputLayout> GetInputLayout() {
			auto inputLayout = InputLayout::Create();
			inputLayout->Add(EShaderDataType::_float3, "POSITION", 0);
			inputLayout->Add(EShaderDataType::_float3, "NORMAL", 0);
			inputLayout->Add(EShaderDataType::_float3, "TANGENT", 0);
			inputLayout->Add(EShaderDataType::_float3, "BITANGENT", 0);
			inputLayout->Add(EShaderDataType::_float2, "TEXCOORD", 0);
			return inputLayout;
		}
	};

	struct JGBone
	{

	};


	class IAssetStock
	{
	public:
		virtual ~IAssetStock() = default;
	};


	class StaticMeshAssetStock : public IAssetStock, public ISerializable
	{
	public:
		String Name;
		bool   IsSkinned = false;
		List<String>                SubMeshNames;
		List<List<JGVertex>>   Vertices;
		List<u32>		            Indices;
	public:
		virtual ~StaticMeshAssetStock() = default;
	protected:
		virtual void Serialize(FileStreamWriter* writer) const override;
		virtual void DeSerialize(FileStreamReader* reader) override;
	};

#define ASSET_NULL_ID -1
	class IAsset : public IJGObject
	{
	public:
		virtual u64 GetAssetID() const = 0;
		virtual const String& GetAssetFullPath() const = 0;
		virtual const String& GetAssetPath() const     = 0;
		virtual const String& GetAssetName() const     = 0;
		virtual const String& GetExtension() const = 0;
	public:
		virtual ~IAsset() = default;
	};

	template<class T>
	class Asset : public IAsset
	{
		JGCLASS
		friend class AssetManager;
		u64    mAssetID = ASSET_NULL_ID;
		String mAssetPath;
		String mAssetFullPath;
		String mExtension;
		String mName;

		SharedPtr<T> mData = nullptr;
	public:
		virtual u64 GetAssetID() const override
		{
			return mAssetID;
		}
		virtual const String& GetAssetFullPath() const override
		{
			return mAssetFullPath;
		}
		virtual const String& GetAssetPath() const override
		{
			return mAssetPath;
		}
		virtual const String& GetAssetName() const override
		{
			return mName;
		}
		virtual const String& GetName() const override
		{
			return mName;
		}
		virtual const String& GetExtension() const override
		{
			return mExtension;
		}

		T* Get() const
		{
			mData.get();
		}
	public:
		virtual ~Asset() = default;
	};


	class AssetManager;
	class AssetDataBase : public GlobalSingleton<AssetDataBase>
	{
		friend class Application;
		Dictionary<AssetManager*, SharedPtr<AssetManager>> mAssetManagerPool;
		HashSet<SharedPtr<AssetManager>> mWaitingAssetManager;
	public:
		AssetDataBase();
		virtual ~AssetDataBase();
	public:
		SharedPtr<AssetManager> RequestAssetManager();
		void ReturnAssetManager(SharedPtr<AssetManager> assetManager);
	};
}