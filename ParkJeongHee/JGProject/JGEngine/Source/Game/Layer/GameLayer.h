#pragma once
#include "JGCore.h"
#include "Class/Layer.h"

namespace JG
{
	class GameNode;
	class GameRootNode;
	class GameLayer : public ILayer
	{
		GameRootNode* mGameRootNode = nullptr;
	public:
		virtual ~GameLayer() {}
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void Begin() override;
		virtual void Destroy() override;

		virtual void OnEvent(IEvent& e) override;

		virtual String GetLayerName() override;
	public:
		EScheduleResult WorldHierarchyUpdate();
	};
}