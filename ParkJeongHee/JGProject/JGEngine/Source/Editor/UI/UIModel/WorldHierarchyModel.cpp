#include "pch.h"
#include "WorldHierarchyModel.h"


namespace JG
{
	void WorldHierarchyModel::Initialize()
	{

	}

	void WorldHierarchyModel::Destroy()
	{
		mGameWorld = nullptr;
	}

	void WorldHierarchyModel::SetGameWorld(GameWorld* gameWorld)
	{
		mGameWorld = gameWorld;
	}

	GameWorld* WorldHierarchyModel::GetGameWorld() const
	{
		return mGameWorld;
	}
}

