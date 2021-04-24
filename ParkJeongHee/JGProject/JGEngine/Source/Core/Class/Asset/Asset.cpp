#include "pch.h"
#include "Asset.h"
#include "AssetManager.h"

namespace JG
{
	void StaticMeshAssetStock::Serialize(FileStreamWriter* writer) const {
		writer->Write(Name);
		writer->Write(IsSkinned);
		writer->Write(SubMeshNames);
		writer->Write(Vertices);
		writer->Write(Indices);

	}
	void StaticMeshAssetStock::DeSerialize(FileStreamReader* reader) {
		reader->Read(&Name);
		reader->Read(&IsSkinned);
		reader->Read(&SubMeshNames);
		reader->Read(&Vertices);
		reader->Read(&Indices);

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