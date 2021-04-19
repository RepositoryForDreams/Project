#pragma once
#include "Common/Enum.h"
#include "Common/Type.h"
#include "Common/Abstract.h"

#include <string>



namespace JG
{
	//enum class EEventType
	//{
	//	// Application
	//	AppOpen, AppClose,  AppResize,

	//	// Input
	//	KeyPressed, KeyReleased, KeyTyping,
	//	MouseMoved, MouseButtonPressed, MouseButtonReleased,
	//	MouseScrollMoved,

	//	// Game
	//};


	ENUM_FLAG(EEventCategory)
	enum class EEventCategory
	{
		None			= 0,
		Application		= 0x001,
		Keyboard		= 0x002,
		Mouse			= 0x004,
		MouseButton		= 0x008,
		Input			= 0x010,
		Game            = 0x020,
		Editor          = 0x030,

		Request         = 0x100,
		Response        = 0x200,
		Destroy         = 0x400,
	};

#define  EVENTCLASS(category) \
public: \
	virtual Type GetEventType()  const override { return Type(TypeID(this)); } \
	virtual EEventCategory GetCategory() const override { return category; } \
private: \


#define EVENT_BIND_FN(func) std::bind(func, this, std::placeholders::_1)

	
	using EventResult = ptraddr;
	class IEvent
	{
	public:
		virtual ~IEvent() = default;
		virtual String    ToString()    const = 0;
		virtual Type      GetEventType()const = 0;
		virtual EEventCategory GetCategory() const = 0;
	public:
		bool IsInCategory(EEventCategory category) const
		{
			return GetCategory() & category;
		}

	public:
		bool	    Handled = false;
		EventResult Result  = 0;
	};

	class EventDispatcher
	{
	private:
		IEvent& mEvent;
	public:
		EventDispatcher(IEvent& e) : mEvent(e) {}
	public:
		template<class T, class Func>
		bool Dispatch(const Func& func)
		{
			if(mEvent.GetEventType() == Type(TypeID<T>()))
			{
				mEvent.Handled |= func(static_cast<T&>(mEvent));
				return true;
			}
			return false;
		}
	};
}