#include "pch.h"
#include "GameLayerManager.h"

namespace JG
{
	u64 GameLayer::GetMask(const String& layerName)
	{
		return GameLayerManager::GetInstance().GetMask(layerName);
	}


	GameLayerManager::GameLayerManager()
	{
		RegisterGameLayer(TT("Default"));
	}

	void GameLayerManager::RegisterGameLayer(const String& layerName)
	{
		if (mGameLayerByName.find(layerName) != mGameLayerByName.end())
		{
			return;
		}

		auto layer = CreateUniquePtr<GameLayer>();
		layer->Name = layerName;

		auto player = layer.get();
		mGameLayerList.push_back(std::move(layer));
		mGameLayerByName[player->Name] = player;
		MakeLayerMask();
	}
	void GameLayerManager::UnRegisterGameLayer(const String& layerName)
	{
		if (mGameLayerByName.find(layerName) == mGameLayerByName.end())
		{
			return;
		}

		for (auto& iter = mGameLayerList.begin(); iter != mGameLayerList.end();)
		{
			if ((*iter)->Name == layerName)
			{
				iter = mGameLayerList.erase(iter);
				break;
			}
			else
			{
				++iter;
			}
		}
		MakeLayerMask();
	}
	u64 GameLayerManager::GetMask(const String& layerName) const
	{
		if (mGameLayerByName.find(layerName) == mGameLayerByName.end())
		{
			return mGameLayerMask.at(TT("Default"));
		}

		return mGameLayerMask.at(layerName);
	}
	void GameLayerManager::MakeLayerMask()
	{
		mGameLayerMask.clear();
		u64 mask = 1;
		for (auto& layer : mGameLayerList)
		{
			mGameLayerMask[layer->Name] = mask;
			mask << 1;
		}
	}

}


