#pragma once
#include "Common/Define.h"
#include "Common/Enum.h"
#include "Common/Abstract.h"
#include "Common/Type.h"
#include "Graphics/Resource.h"
#include "Class/Global/Scheduler.h"
#include "Class/FileIO.h"
namespace JG
{
	struct JGVertex
	{
		JVector3 Position;
		JVector2 Texcoord;
		JVector3 Normal;
		JVector3 Tangent;
		JVector3 Bitangent;


		static SharedPtr<InputLayout> GetInputLayout() {
			auto inputLayout = InputLayout::Create();
			inputLayout->Add(EShaderDataType::_float3, "POSITION", 0);
			inputLayout->Add(EShaderDataType::_float2, "TEXCOORD", 0);
			inputLayout->Add(EShaderDataType::_float3, "NORMAL", 0);
			inputLayout->Add(EShaderDataType::_float3, "TANGENT", 0);
			inputLayout->Add(EShaderDataType::_float3, "BITANGENT", 0);
			return inputLayout;
		}
	public:
		void MakeJson(SharedPtr<JsonData> jsonData) const 
		{
			auto& val = jsonData->GetValue();
			val.SetArray();
			val.PushBack(Position.x, jsonData->GetJsonAllocator());
			val.PushBack(Position.y, jsonData->GetJsonAllocator());
			val.PushBack(Position.z, jsonData->GetJsonAllocator());
			val.PushBack(Texcoord.x, jsonData->GetJsonAllocator());
			val.PushBack(Texcoord.y, jsonData->GetJsonAllocator());
			val.PushBack(Normal.x, jsonData->GetJsonAllocator());
			val.PushBack(Normal.y, jsonData->GetJsonAllocator());
			val.PushBack(Normal.z, jsonData->GetJsonAllocator());
			val.PushBack(Tangent.x, jsonData->GetJsonAllocator());
			val.PushBack(Tangent.y, jsonData->GetJsonAllocator());
			val.PushBack(Tangent.z, jsonData->GetJsonAllocator());
			val.PushBack(Bitangent.x, jsonData->GetJsonAllocator());
			val.PushBack(Bitangent.y, jsonData->GetJsonAllocator());
			val.PushBack(Bitangent.z, jsonData->GetJsonAllocator());
		}
		void LoadJson(SharedPtr<JsonData> jsonData)
		{
			auto& val = jsonData->GetValue();
			i32 index = 0;
			for (auto& value : val.GetArray())
			{
				f32 f = value.GetFloat();
				if (index < 3)
				{
					Position[index] = f;
				}
				else if (index < 5)
				{
					Texcoord[index - 3] = f;
				}
				else if (index < 8)
				{
					Normal[index - 5] = f;
				}
				else if (index < 11)
				{
					Tangent[index - 8] = f;
				}
				else
				{
					Bitangent[index - 11] = f;
				}
				++index;
			}
		}
	};
	struct JGQuadVertex
	{
		JVector3 Position;
		JVector2 Texcoord;


		static SharedPtr<InputLayout> GetInputLayout() {
			auto inputLayout = InputLayout::Create();
			inputLayout->Add(EShaderDataType::_float3, "POSITION", 0);
			inputLayout->Add(EShaderDataType::_float2, "TEXCOORD", 0);
			return inputLayout;
		}
	public:
		void MakeJson(SharedPtr<JsonData> jsonData) const 
		{
			jsonData->AddMember("Position", Position);
			jsonData->AddMember("Texcoord", Texcoord);
		}
		void LoadJson(SharedPtr<JsonData> jsonData)
		{
			auto val = jsonData->GetMember("Position");
			if (val)
			{
				Position = val->GetVector3();
			}
			val = jsonData->GetMember("Texcoord");
			if (val)
			{
				Texcoord = val->GetVector2();
			}
		}
	};
	struct JGBone
	{

	};


	class IAssetStock : public IJson
	{
	public:
		virtual ~IAssetStock() = default;
		virtual EAssetFormat GetAssetFormat() const = 0;
	};

	class TextureAssetStock :  public IAssetStock
	{
	public:
		String Name;
		i32 Width    = 0;
		i32 Height   = 0;
		i32 Channels = 0;
		u32 PixelPerUnit = 100;
		List<jbyte> Pixels;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual ~TextureAssetStock() = default;
	public:
		virtual EAssetFormat GetAssetFormat() const  override {
			return EAssetFormat::Texture;
		}
	};


	class StaticMeshAssetStock : public IAssetStock
	{
	public:
		String Name;
		bool   IsSkinned = false;
		List<String>           SubMeshNames;
		List<List<JGVertex>>   Vertices;
		List<List<u32>>		   Indices;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual ~StaticMeshAssetStock() = default;
	public:
		virtual EAssetFormat GetAssetFormat() const override {
			return EAssetFormat::Mesh;
		}
	};


	class MaterialAssetStock : public IAssetStock
	{
	public:
		String Name;
		// ShaderScript
	};


	

#define ASSET_NULL_ID -1

	class AssetID
	{
		friend class AssetManager;
	private:
		u64  Origin = ASSET_NULL_ID;
		u64  ID = ASSET_NULL_ID;
		bool ReadWrite = false;
	public:
		u64 GetID() const {
			return ID;
		}
		u64 GetOriginID() const {
			return Origin;
		}
		bool IsReadWrite() const {
			return ReadWrite;
		}
		bool IsValid() const {
			return ID != ASSET_NULL_ID;
		}
	};


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
		Asset(const String& assetPath)
		{
			fs::path p(assetPath);
			fs::path contentsPath = fs::absolute(Application::GetAssetPath()).wstring();
			mAssetID = (u64)this;
			mAssetFullPath = fs::absolute(assetPath).wstring();
			mAssetPath = ReplaceAll(mAssetFullPath, contentsPath, TT(""));
			mExtension = p.extension();
			mName = ReplaceAll(p.wstring() ,mExtension, TT(""));
		}
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
		virtual const String& GetExtension() const override
		{
			return mExtension;
		}
		SharedPtr<T> Get() const {
			return mData;
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