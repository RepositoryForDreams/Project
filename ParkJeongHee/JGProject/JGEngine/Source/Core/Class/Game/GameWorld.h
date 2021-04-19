#pragma once
#include "GameNode.h"

namespace JG
{
	class GameWorld : public GameNode
	{
		GAMECLASS
	public:
		GameWorld();
	public:
		virtual void SetParent(GameNode* node) override {}
		virtual void OnInspectorGUI() override {}
	};
}