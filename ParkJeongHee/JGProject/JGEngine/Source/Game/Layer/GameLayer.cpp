#include "pch.h"
#include "GameLayer.h"
#include "Application.h"
#include "Class/Game/GameNode.h"



namespace JG
{
	void GameLayer::OnAttach()
	{
		GameObjectFactory::Create();
	}
	void GameLayer::OnDetach()
	{
		GameObjectFactory::Destroy();
	}
	void GameLayer::Begin()
	{
		Scheduler::GetInstance().Schedule(0.0f, 0.16f, -1, SchedulePriority::GameLayer, SCHEDULE_BIND_FN(&GameLayer::WorldHierarchyUpdate));

		mGameRootNode = GameObjectFactory::GetInstance().CreateObject<GameRootNode>();
	}
	void GameLayer::Destroy()
	{

	}
	void GameLayer::OnEvent(IEvent& e)
	{
	}
	String GameLayer::GetLayerName()
	{
		return TT("GameLayer");
	}
	EScheduleResult GameLayer::WorldHierarchyUpdate()
	{
		WorldHierarchyUpdateEvent e;
		e.RootNode = mGameRootNode;
		Application::GetInstance().SendEvent(e);
		return EScheduleResult::Continue;
	}
}