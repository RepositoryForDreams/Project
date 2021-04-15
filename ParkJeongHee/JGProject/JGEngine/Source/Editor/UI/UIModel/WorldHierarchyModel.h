#pragma once
#include "Class/UI/UIModel.h"


namespace JG
{
	class GameWorld;
	class WorldHierarchyModel : public UIModel
	{
		GameWorld* mGameWorld = nullptr;
	public:
		virtual ~WorldHierarchyModel() = default;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;
	public:
		void SetGameWorld(GameWorld* gameWorld);
		GameWorld* GetGameWorld() const;
	};
}