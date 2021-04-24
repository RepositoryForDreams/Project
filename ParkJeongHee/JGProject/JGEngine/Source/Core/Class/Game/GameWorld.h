#pragma once
#include "GameNode.h"
#include "Class/Asset/AssetManager.h"

namespace JG
{
	class GameWorld : public GameNode
	{
		GAMECLASS
	private:
		SharedPtr<AssetManager> mAssetManager;
	public:
		GameWorld();
	public:
		virtual void SetParent(GameNode* node) override {}
		virtual void OnInspectorGUI() override {}
	};
}