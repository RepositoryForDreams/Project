#pragma once
#include "Events.h"





namespace JG
{
	class KeyEvent : public IEvent
	{
	public:
		EKeyCode KeyCode;
	};


	class KeyPressedEvent : public KeyEvent
	{
		EVENT_CLASS(EEventType::KeyPressed, EEventCategory::Keyboard | EEventCategory::Input)
	public:
		virtual ~KeyPressedEvent() override  {}
		virtual String ToString() const override
		{
			return TT("KeyPressed : ") + KeyCodeToString(KeyCode);
		}

		
	};

	class KeyReleasedEvent : public KeyEvent
	{
		EVENT_CLASS(EEventType::KeyPressed, EEventCategory::Keyboard | EEventCategory::Input)
	public:
		virtual ~KeyReleasedEvent() override  {}
		virtual String ToString() const override
		{
			return TT("KeyReleased : ") + KeyCodeToString(KeyCode);
		}
	};


	class KeyTypingEvent : public IEvent
	{
		EVENT_CLASS(EEventType::KeyTyping, EEventCategory::Keyboard | EEventCategory::Input)
	public:
		virtual ~KeyTypingEvent() override {}
		virtual String ToString() const override
		{
			return TT("KeyTyping : ") + String(&Key);
		}
	public:
		wchar Key;
	};
	

	
}