#pragma once
#include "Common/Define.h"
#include "Asset.h"
namespace JG
{
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
	};
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
		SharedPtr<T> GetAsset(AssetID id)
		{
			std::shared_lock<std::shared_mutex> lock(mMutex);

			if (id.IsReadWrite() == false)
			{
				auto iter = mAssetPoolByID.find(id.GetID());
				if (iter == mAssetPoolByID.end())
				{
					return nullptr;
				}

				return iter->second;
			}
			else
			{
				auto iter = mRWAssetPool.find(id.GetID());
				if (iter == mRWAssetPool.end())
				{
					return nullptr;
				}

				return iter->second;
			}
		}

		template<class T>
		SharedPtr<T> GetOriginAsset(AssetID id)
		{
			std::shared_lock<std::shared_mutex> lock(mMutex);

			auto iter = mAssetPoolByID.find(id.GetOriginID());
			if (iter == mAssetPoolByID.end())
			{
				return nullptr;
			}

			return iter->second;
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