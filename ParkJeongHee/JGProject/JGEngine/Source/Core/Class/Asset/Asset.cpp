#include "pch.h"
#include "Asset.h"
#include "AssetManager.h"

namespace JG
{
	void TextureAssetStock::Serialize(FileStreamWriter* writer) const
	{
		writer->Write<String>(TT("Name"), Name);
		writer->Write<i32>(TT("Width"), Width);
		writer->Write<i32>(TT("Height"), Height);
		writer->Write<i32>(TT("Channels"), Channels);
		writer->Write(TT("Pixels"), Pixels);
	}

	void TextureAssetStock::DeSerialize(FileStreamReader* reader)
	{
		reader->Read(TT("Name"), &Name);
		reader->Read(TT("Width"), &Width);
		reader->Read(TT("Height"), &Height);
		reader->Read(TT("Channels"), &Channels);
		reader->Read(TT("Pixels"), &Pixels);
	}


	void StaticMeshAssetStock::Serialize(FileStreamWriter* writer) const {
		writer->Write(TT("Name"), Name);
		writer->Write<bool>(TT("IsSkinned"), IsSkinned);
		writer->Write(TT("SubMeshNames"), SubMeshNames);
		writer->Write(TT("Vertices"), Vertices);
		writer->Write(TT("Indices"), Indices);

	}
	void StaticMeshAssetStock::DeSerialize(FileStreamReader* reader) {
		reader->Read(TT("Name"), &Name);
		reader->Read(TT("IsSkinned") , &IsSkinned);
		reader->Read(TT("SubMeshNames"), &SubMeshNames);
		reader->Read(TT("Vertices"), &Vertices);
		reader->Read(TT("Indices"), &Indices);

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