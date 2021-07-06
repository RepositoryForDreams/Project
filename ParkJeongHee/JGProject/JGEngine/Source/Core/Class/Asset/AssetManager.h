#pragma once
#include "Common/Define.h"
#include "Asset.h"
namespace JG
{
	class AssetManager
	{
		struct LoadingData
		{
			String Path;
			SharedPtr<IAsset> Asset;
		};

		struct AssetContext
		{
			u64 OriginAssetID = ASSET_NULL_ID;
			SharedPtr<IAsset> RWAsset;
		};

		Dictionary<String, SharedPtr<IAsset>>   mAssetPool;
		Dictionary<u64, SharedPtr<IAsset>>		mAssetPoolByID;
		Dictionary<String, u64>				    mAssetIDPool;

		Dictionary<u64, SharedPtr<IAsset>> mRWAssetPool;
		



		Dictionary<String, SharedPtr<LoadingData>> mLoadingAssetPool;
		Queue<String> mWaitingLoadAsset;
		std::shared_mutex mMutex;
		bool mIsResetting = false;

	public:
		AssetID  AsyncLoadAsset(const String& path);
		AssetID  RequestReadWriteAsset(const String& path);
		void Reset();
		bool IsResetting() const;
	public:
		template<class T>
		Asset<T>* GetAsset(AssetID id)
		{
			std::shared_lock<std::shared_mutex> lock(mMutex);

			if (id.IsReadWrite() == false)
			{
				auto iter = mAssetPoolByID.find(id.GetID());
				if (iter == mAssetPoolByID.end())
				{
					return nullptr;
				}

				return static_cast<Asset<T>*>(iter->second.get());
			}
			else
			{
				auto iter = mRWAssetPool.find(id.GetID());
				if (iter == mRWAssetPool.end())
				{
					return nullptr;
				}

				return static_cast<Asset<T>*>(iter->second.get());
			}
		}

		template<class T>
		Asset<T>* GetOriginAsset(AssetID id)
		{
			std::shared_lock<std::shared_mutex> lock(mMutex);

			auto iter = mAssetPoolByID.find(id.GetOriginID());
			if (iter == mAssetPoolByID.end())
			{
				return nullptr;
			}

			return static_cast<Asset<T>*>(iter->second.get());
		}

		template<class T>
		AssetID GetOriginAssetID(const String& path)
		{
			std::shared_lock<std::shared_mutex> lock(mMutex);

			auto iter = mAssetIDPool.find(id.GetOriginID());
			if (iter == mAssetIDPool.end())
			{
				return AssetID();
			}
			AssetID id;
			id.Origin = iter->second;
			id.ID = iter->second;
			id.ReadWrite = false;
			return id;
		}
	private:
		SharedPtr<IAsset> LoadAsset(const String path);
	};
}