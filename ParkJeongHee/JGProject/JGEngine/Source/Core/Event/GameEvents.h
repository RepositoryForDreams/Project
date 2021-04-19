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

	class NotifyDestroyGameObjectEvent : public GameEvent
	{
		EVENTCLASS(EEventCategory::Editor | EEventCategory::Destroy)
	public:
		u64 DestroyedGameObjectID = 0;
	public:
		virtual ~NotifyDestroyGameObjectEvent() = default;
		virtual String ToString() const override
		{
			return TT("NotifyDestroyGameObjectEvent");
		}
	};
}