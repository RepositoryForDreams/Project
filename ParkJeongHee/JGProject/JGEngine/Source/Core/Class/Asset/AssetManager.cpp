#include "pch.h"
#include "AssetManager.h"
#include "Application.h"
#include "Class/FileIO.h"
#include "Graphics/Mesh.h"
namespace JG
{
	AssetID AssetManager::AsyncLoadAsset(const String& path)
	{
		if (mIsResetting == true)
		{
			mWaitingLoadAsset.push(path);
		}


		{
			std::shared_lock<std::shared_mutex> lock(mMutex);
			if (mAssetPool.find(path) != mAssetPool.end())
			{
				AssetID id;
				id.Origin    = mAssetIDPool[path];
				id.ID        = id.Origin;
				id.ReadWrite = false;
				return id;
			}
		}

		// 로딩 중이라면 false
		if (mLoadingAssetPool.find(path) != mLoadingAssetPool.end())
		{
			return AssetID();
		}

		{
			std::lock_guard<std::shared_mutex> lock(mMutex);
			mLoadingAssetPool.emplace(path, CreateSharedPtr<LoadingData>());
			mLoadingAssetPool[path]->Path = path;
		}
		
		
		Scheduler::GetInstance().ScheduleAsync(
			[&](void* userData)
		{
			LoadingData* loadingData = (LoadingData*)userData;
			loadingData->Asset = LoadAsset(loadingData->Path);
			
			{
				std::lock_guard<std::shared_mutex> lock(mMutex);
				if (loadingData->Asset != nullptr)
				{
					mAssetPool.emplace(loadingData->Path, loadingData->Asset);
					mAssetPoolByID.emplace(loadingData->Asset->GetAssetID(), loadingData->Asset);
					mAssetIDPool.emplace(loadingData->Path, loadingData->Asset->GetAssetID());
				}
				mLoadingAssetPool.erase(loadingData->Path);
			}
		}, mLoadingAssetPool[path].get());
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
			
			std::lock_guard<std::shared_mutex> lock(mMutex);
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
				AsyncLoadAsset(path);
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
	SharedPtr<IAsset> AssetManager::LoadAsset(const String path)
	{
		fs::path assetPath = path;
		if (fs::exists(assetPath) == false)
		{
			return nullptr;
		}
		EAssetFormat assetFormat = EAssetFormat::None;

		auto json = CreateSharedPtr<Json>();
		if (Json::Read(assetPath, json) == false)
		{
			JG_CORE_ERROR("Fail Load Asset : {0}", assetPath.wstring());
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
			auto textureAsset = CreateSharedPtr<Asset<ITexture>>(assetPath);
			textureAsset->mData = ITexture::Create(stock);
			return textureAsset;
		}
		default:
			JG_CORE_ERROR("{0} AssetFormat is not supported in LoadAsset", (int)assetFormat);
			return nullptr;
		}
	}
}