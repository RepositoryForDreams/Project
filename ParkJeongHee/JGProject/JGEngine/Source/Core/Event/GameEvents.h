#pragma once
#include "Events.h"


namespace JG
{
	class GameEvent : public IEvent
	{

	};

	class ResponseGameWorldEvent : public GameEvent
	{
		EVENTCLASS(EEventCategory::Editor | EEventCategory::Response)
	public:
		class GameWorld* GameWorld = nullptr;
	public:
		virtual ~ResponseGameWorldEvent() = default;
		virtual String ToString() const override
		{
			return TT("ResponseGameWorldEvent");
		}
	};
}