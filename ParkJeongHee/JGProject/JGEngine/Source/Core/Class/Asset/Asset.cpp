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
		jsonData->AddMember("BoundingBox", BoundingBox);

		auto meshJson = jsonData->CreateJsonData();
		auto cnt = SubMeshNames.size();
		for (auto i = 0; i < cnt; ++i)
		{
			auto viJson = meshJson->CreateJsonData();
			viJson->AddMember("Name", SubMeshNames[i]);



			auto vArrayJson = viJson->CreateJsonData();
			for (auto& v : Vertices[i])
			{
				auto vertexJson = viJson->CreateJsonData();
				v.MakeJson(vertexJson);
				vArrayJson->AddMember(vertexJson);
			}

			viJson->AddMember("Vertices", vArrayJson);
			viJson->AddMember("Indices", Indices[i]);


			meshJson->AddMember(viJson);
		}
		jsonData->AddMember("SubMeshs", meshJson);
	}

	void StaticMeshAssetStock::LoadJson(SharedPtr<JsonData> jsonData)
	{
		SubMeshNames.clear();
		Vertices.clear();
		Indices.clear();
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
		val = jsonData->GetMember("BoundingBox");
		if (val)
		{
			BoundingBox = val->GetBoundingBox();
		}
		val = jsonData->GetMember("SubMeshs");
		if (val)
		{
			auto cnt = val->GetSize();
			Vertices.resize(cnt);
			Indices.resize(cnt);
			SubMeshNames.resize(cnt);
			for (auto i = 0; i < cnt; ++i)
			{
				auto meshJson = val->GetJsonDataFromIndex(i);

				auto verticesJson = meshJson->GetMember("Vertices");
				auto indicesJson  = meshJson->GetMember("Indices");
				auto meshName     = meshJson->GetMember("Name");

				SubMeshNames[i] = meshName->GetString();
				Vertices[i].resize(verticesJson->GetSize());
				Indices[i].resize(indicesJson->GetSize());
				for (auto j = 0; j < verticesJson->GetSize(); ++j)
				{
					auto vJson = verticesJson->GetJsonDataFromIndex(j);
					JGVertex v;
					v.LoadJson(vJson);
					Vertices[i][j] = v;
				}
				for (auto j = 0; j < indicesJson->GetSize(); ++j)
				{
					auto iJson = indicesJson->GetJsonDataFromIndex(j);
					u32 index;
					index = iJson->GetUint32();
					Indices[i][j] = index;
				}

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