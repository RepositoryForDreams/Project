#pragma once
#include "Events.h"
#include "Common/Define.h"


namespace JG
{
	class MouseMovedEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Mouse)
	public:
		virtual ~MouseMovedEvent() override {}
		virtual String ToString() const override
		{
			return TT("MouseMovedEvent : [ X : ") + std::to_wstring(X) + TT(", Y : ") + std::to_wstring(Y) + TT(" ]");
		}

	public:
		f64 X = 0;
		f64 Y = 0;
	};


	class MouseButtonPressedEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Input | EEventCategory::MouseButton | EEventCategory::Mouse)
	public:
		virtual ~MouseButtonPressedEvent() override {}
		virtual String ToString() const override
		{
			return TT("MouseButtonPressed : ") + MouseButtonToString(MouseButton);
		}
	public:
		EMouseButton MouseButton;
	};

	class MouseButtonReleasedEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Input | EEventCategory::MouseButton | EEventCategory::Mouse)
	public:
		virtual ~MouseButtonReleasedEvent() override {}
		virtual String ToString() const override
		{
			return TT("MouseButtonReleased : ") + MouseButtonToString(MouseButton);
		}
	public:
		EMouseButton MouseButton;
	};

	class MouseScrollMovedEvent : public IEvent
	{
		EVENTCLASS( EEventCategory::Mouse);
	public:
		virtual ~MouseScrollMovedEvent(){}
		virtual String ToString() const override
		{
			return TT("MouseScrollMoved Offset : ") + std::to_wstring(Offset) + TT("  HOffset : ") + std::to_wstring(HOffset);
		}
	public:
		f64 Offset  = 0.0f;
		f64 HOffset = 0.0f;
	};
	
	
}
