#pragma once
#include "Events.h"


namespace JG
{
	class GameEvent : public IEvent
	{

	};


	class WorldHierarchyUpdateEvent : public GameEvent
	{
		EVENT_CLASS(EEventType::WorldHierarchyUpdate, EEventCategory::Game)
	public:
		class GameRootNode* RootNode = nullptr;
	public:
		virtual ~WorldHierarchyUpdateEvent() override {}
		virtual String ToString() const override
		{
			return TT("WorldHierarchyUpdateEvent");
		}
	};
}