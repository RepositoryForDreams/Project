#include "pch.h"
#include "Asset.h"
#include "AssetManager.h"
namespace JG
{
	void TextureAssetStock::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		jsonData->AddMember("Name", Name);
		jsonData->AddMember("Width", Width);
		jsonData->AddMember("Height", Height);
		jsonData->AddMember("Channels", Channels);
		jsonData->AddMember("PixelPerUnit", PixelPerUnit);
		jsonData->AddMember("Pixels", Pixels);

	}

	void TextureAssetStock::LoadJson(SharedPtr<JsonData> jsonData)
	{
		auto Val = jsonData->GetMember("Name");
		if (Val && Val->IsString())
		{
			Name = Val->GetString();
		}
		Val = jsonData->GetMember("Width");
		if (Val && Val->IsInt32())
		{
			Width = Val->GetInt32();
		}
		Val = jsonData->GetMember("Height");
		if (Val && Val->IsInt32())
		{
			Height = Val->GetInt32();
		}
		Val = jsonData->GetMember("Channels");
		if (Val && Val->IsInt32())
		{
			Channels = Val->GetInt32();
		}
		Val = jsonData->GetMember("PixelPerUnit");
		if (Val && Val->GetUint32())
		{
			PixelPerUnit = Val->GetUint32();
		}

		Val = jsonData->GetMember("Pixels");
		if (Val && Val->IsByteList())
		{
			Pixels = Val->GetByteList();
		}
	}
	void StaticMeshAssetStock::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		jsonData->AddMember("Name", Name);
		jsonData->AddMember("IsSkinned", IsSkinned);
		

		auto meshJson = jsonData->CreateJsonData();

		


		auto cnt = SubMeshNames.size();
		for (auto i = 0; i < cnt; ++i)
		{
			meshJson->AddMember(SubMeshNames[i], Vertices[i]);
		}

		jsonData->AddMember("Vertices", meshJson);
		jsonData->AddMember("Indices", Indices);
	}

	void StaticMeshAssetStock::LoadJson(SharedPtr<JsonData> jsonData)
	{
		SubMeshNames.clear();
		Vertices.clear();
		auto val = jsonData->GetMember("Name");
		if (val)
		{
			Name = val->GetString();
		}
		val = jsonData->GetMember("IsSkinned");
		if (val)
		{
			IsSkinned = val->GetBool();
		}

		val = jsonData->GetMember("Vertices");
		u64 size = val->GetSize();
		SubMeshNames.resize(size);
		Vertices.resize(size);
		for (i32 i = 0; i < size; ++i)
		{
			auto meshVal = val->GetJsonDataFromIndex(i);
			auto nameVal = meshVal->GetJsonDataFromIndex(0);
			auto versVal = meshVal->GetJsonDataFromIndex(1);

			auto name = nameVal->GetString();
			SubMeshNames[i] = name;
			Vertices[i].resize(versVal->GetSize());
			for (i32 j = 0; j < versVal->GetSize(); ++j)
			{
				auto verVal = versVal->GetJsonDataFromIndex(j);
				JGVertex v;
				v.LoadJson(verVal);
				Vertices[i].push_back(v);
			}
		}
	}




	AssetDataBase::AssetDataBase()
	{
	}
	AssetDataBase::~AssetDataBase()
	{
		mWaitingAssetManager.clear();
		mAssetManagerPool.clear();
	}
	SharedPtr<AssetManager> AssetDataBase::RequestAssetManager()
	{
		SharedPtr<AssetManager> result = nullptr;
		if (mWaitingAssetManager.empty() == false)
		{
			
			for (auto& assetManager : mWaitingAssetManager)
			{
				if (assetManager->IsResetting() == false)
				{
					result = assetManager;
					break;
				}
			}

			if (result != nullptr)
			{
				mAssetManagerPool[result.get()] = result;
				mWaitingAssetManager.erase(result);
				return result;
			}

		}
		
		result = CreateSharedPtr<AssetManager>();
		mAssetManagerPool[result.get()] = result;


		return result;
	}
	void AssetDataBase::ReturnAssetManager(SharedPtr<AssetManager> assetManager)
	{
		assetManager->Reset();
		mWaitingAssetManager.insert(assetManager);
		mAssetManagerPool.erase(assetManager.get());
	}



}