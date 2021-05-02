#pragma once


#include "JGCore.h"

namespace JG
{
	class GameLayer
	{
		friend class GameLayerManager;
	public:
		static u64 GetMask(const String& layerName);
	private:
		String Name;
	};

	class GameLayerManager : public GlobalSingleton<GameLayerManager>
	{
		friend class Application;
	private:
		List<UniquePtr<GameLayer>>     mGameLayerList;
		Dictionary<String, GameLayer*> mGameLayerByName;
		Dictionary<String, u64> mGameLayerMask;
	public:
		GameLayerManager();
	public:
		void RegisterGameLayer(const String& layerName);
		void UnRegisterGameLayer(const String& layerName);
		u64 GetMask(const String& layerName) const;
	private:
		void MakeLayerMask();
	};


}