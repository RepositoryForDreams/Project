#include "pch.h"
#include "GameLayerSystem.h"

namespace JG
{
	void GameLayerSystem::AddGameLayer(const String& name, i64 priority)
	{
		auto layer      = CreateUniquePtr<GameLayer>();
		layer->Name     = name;
		layer->Priority = priority;

	
	}
	void GameLayerSystem::RemoveGameLayer(const String& name)
	{
	}
	GameLayer* GameLayerSystem::GetGameLayer(const String& name) const
	{
		return nullptr;
	}
}