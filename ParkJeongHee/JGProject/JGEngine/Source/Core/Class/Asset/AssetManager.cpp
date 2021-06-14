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
		fs::path contentsPath = Application::GetAssetPath();

		if (fs::exists(assetPath) == false)
		{
			return nullptr;
		}
		EAssetFormat assetFormat = EAssetFormat::None;
		FileStreamReader reader;
		if (reader.Open(assetPath) == true)
		{
			reader.Read(JG_ASSET_FORMAT_KEY , &assetFormat);

			switch (assetFormat)
			{
			case EAssetFormat::Texture:
			{
				TextureAssetStock stock;
				reader.Read(JG_TEXTURE_ASSET_KEY , &stock);

				// TODO
				// LoadTexture
			}
				break;
			case EAssetFormat::Mesh:
			{
				StaticMeshAssetStock stock;
				reader.Read(JG_STATIC_MESH_ASSET_KEY , &stock);
				// Load Mesh
			}
				break;
			default:
				return nullptr;
			}
			reader.Close();
		}

		return nullptr;
		//if (assetPath.extension() == ASSET_MESH_FORMAT)
		//{
		//	auto asset = CreateSharedPtr<Asset<IMesh>>();
		//	asset->mData = IMesh::CreateFromFile(path);
		//	if (asset->mData == nullptr)
		//	{
		//		JG_CORE_ERROR("Failed Load Mesh : {0} ", path);
		//		return nullptr;
		//	}
		//	asset->mAssetFullPath = assetPath;
		//	asset->mAssetPath = ReplaceAll(assetPath, contentsPath, TT(""));
		//	asset->mName      = assetPath.filename();
		//	asset->mExtension = assetPath.extension();
		//	asset->mAssetID	  = (u64)asset.get();

		//	return asset;
		//}
		//else
		//{
		//	JG_CORE_ERROR("is not supported file format : {0} ", assetPath.extension().string());
		//	return nullptr;
		//}


	}
}