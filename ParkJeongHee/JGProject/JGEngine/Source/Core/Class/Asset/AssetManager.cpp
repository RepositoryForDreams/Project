#include "pch.h"
#include "AssetManager.h"
#include "Application.h"
#include "Class/FileIO.h"
#include "Graphics/Mesh.h"
namespace JG
{
	AssetManager::AssetManager()
	{
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, 0, [&]()-> EScheduleResult
		{
			for (auto& _pair : mLoadingAssetPool)
			{
				LoadingData* loadingData = _pair.second.get();
				loadingData->Asset = LoadAssetInternal(loadingData->Path);
				if (loadingData->Asset != nullptr)
				{
					mAssetPool.emplace(loadingData->Path, loadingData->Asset);
					mAssetPoolByID.emplace(loadingData->Asset->GetAssetID(), loadingData->Asset);
					mAssetIDPool.emplace(loadingData->Path, loadingData->Asset->GetAssetID());
				}
			}
			mLoadingAssetPool.clear();

			return EScheduleResult::Continue;
		});
	}
	AssetID AssetManager::LoadAsset(const String& path)
	{
		if (mIsResetting == true)
		{
			mWaitingLoadAsset.push(path);
		}

		if (mAssetPool.find(path) != mAssetPool.end())
		{
			AssetID id;
			id.Origin = mAssetIDPool[path];
			id.ID = id.Origin;
			return id;
		}

		if (mLoadingAssetPool.find(path) != mLoadingAssetPool.end())
		{
			return AssetID();
		}
		mLoadingAssetPool.emplace(path, CreateSharedPtr<LoadingData>());
		mLoadingAssetPool[path]->Path = path;
		

		return AssetID();
	}

	AssetID  AssetManager::RequestReadWriteAsset(const String& path)
	{
		return AssetID();
	}
	bool AssetManager::IsResetting() const
	{
		return mIsResetting;
	}
	void AssetManager::Reset()
	{
		if (mIsResetting == true)
		{
			return;
		}
		while (mWaitingLoadAsset.empty() == false)
		{
			mWaitingLoadAsset.pop();
		}
		mIsResetting = true;

		Scheduler::GetInstance().ScheduleAsync(
			[&](void* userData)
		{
			// 로딩중인 에셋이 모두끝날때까지 대기
			while (mLoadingAssetPool.empty() == false) {}
		
			mAssetPool.clear();
			mAssetPoolByID.clear();
			mAssetIDPool.clear();
			mIsResetting = false;
		});

		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::Default,
			[&]() -> EScheduleResult
		{
			if (mIsResetting == true)
			{
				return EScheduleResult::Continue;
			}

			i32 maxLoadAssetCount = 10;
			i32 count = 0;
			while (mWaitingLoadAsset.empty() == false)
			{
				if (count >= maxLoadAssetCount)
				{
					break;
				}
				auto path = mWaitingLoadAsset.front(); mWaitingLoadAsset.pop();
				LoadAsset(path);
				++count;
			}
			
			if (mWaitingLoadAsset.empty() == true)
			{
				return EScheduleResult::Break;
			}
			else
			{
				return EScheduleResult::Continue;
			}
		});
	}
	SharedPtr<IAsset> AssetManager::LoadAssetInternal(const String path)
	{
		fs::path assetPath = path;
		if (fs::exists(assetPath) == false)
		{
			return nullptr;
		}
		EAssetFormat assetFormat = EAssetFormat::None;

		auto json = CreateSharedPtr<Json>();
		if (Json::Read(assetPath.string(), json) == false)
		{
			JG_CORE_ERROR("Fail Load Asset : {0}", assetPath.string());
			return nullptr;
		}
		auto assetFormatVal = json->GetMember(JG_ASSET_FORMAT_KEY);
		if (assetFormatVal)
		{
			assetFormat = (EAssetFormat)assetFormatVal->GetUint64();
		}
		auto assetVal = json->GetMember(JG_ASSET_KEY);
		if (assetVal == nullptr)
		{
			return nullptr;
		}
		switch (assetFormat)
		{
		case EAssetFormat::Texture:
		{
			TextureAssetStock stock;
			stock.LoadJson(assetVal);
			auto textureAsset = CreateSharedPtr<Asset<ITexture>>(assetPath.string());
			textureAsset->mData = ITexture::Create(stock);
			return textureAsset;
		}
		case EAssetFormat::Mesh:
		{
			StaticMeshAssetStock stock;
			stock.LoadJson(assetVal);
			auto meshAsset = CreateSharedPtr<Asset<IMesh>>(assetPath.string());
			meshAsset->mData = IMesh::Create(stock);
			return meshAsset;
		}
		default:
			JG_CORE_ERROR("{0} AssetFormat is not supported in LoadAsset", (int)assetFormat);
			return nullptr;
		}
	}
}