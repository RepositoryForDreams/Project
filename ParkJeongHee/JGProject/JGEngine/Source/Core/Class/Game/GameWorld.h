#pragma once
#include "GameNode.h"

namespace JG
{
	class GameWorld : public GameNode
	{
		GAMECLASS
	public:
		virtual void SetParent(GameNode* node) override {}
	};
}