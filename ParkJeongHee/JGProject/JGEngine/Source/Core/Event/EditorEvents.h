#pragma once
#include "Events.h"



namespace JG
{
	class EditorEvent : public IEvent
	{

	};
	class RequestGameWorldEvent : public EditorEvent
	{
		EVENTCLASS(EEventCategory::Game | EEventCategory::Request)
	public:
		virtual ~RequestGameWorldEvent() = default;
		virtual String ToString() const override
		{
			return TT("RequestGameWorld");
		}
	};

}