#pragma once
#include "Events.h"
#include "Common/Define.h"


namespace JG
{
	class AppOpenEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Application)
	public:
		virtual ~AppOpenEvent() override {}
		virtual String ToString() const override
		{
			return TT("AppOpenEvent");
		}
	};

	class AppCloseEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Application)
	public:
		virtual ~AppCloseEvent() override  {}
		virtual String ToString() const override
		{
			return TT("AppCloseEvent");
		}
	};

	class AppResizeEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Application)
	public:
		virtual ~AppResizeEvent() override {}
		virtual String ToString() const override
		{
			return TT("AppResizeEvent = [ W : ") + std::to_wstring(Width) + TT(", H : ") + std::to_wstring(Height) + TT(" ]");
		}
	public:
		u32 Width  = 0;
		u32 Height = 0;
	};
	
}
