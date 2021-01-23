#pragma once
#include "Common/Enum.h"
#include <string>



namespace JG
{
	enum class EEventType
	{
		AppOpen, AppClose,  AppResize,
		KeyPressed, KeyReleased, KeyTyping,
		MouseMoved, MouseButtonPressed, MouseButtonReleased,
		MouseScrollMoved,
	};


	ENUM_FLAG(EEventCategory)
	enum class EEventCategory
	{
		None			= 0,
		Application		= 1,
		Keyboard		= 2,
		Mouse			= 4,
		MouseButton		= 8,
		Input			= 16,
	};

#define EVENT_TYPE(type) \
	static EEventType GetStaticType() { return type; } \
	virtual EEventType GetEventType() const override { return type; } \

	
#define EVENT_CATEGORY(category) \
	virtual EEventCategory GetCategory() const override { return category; } \


#define  EVENT_CLASS(eventType, category) \
public: \
	EVENT_TYPE(eventType) \
	EVENT_CATEGORY(category) \
private: \


#define EVENT_BIND_FN(func) std::bind(func, this, std::placeholders::_1)

	
	using EventResult = ptraddr;
	class IEvent
	{
	public:
		virtual ~IEvent() = default;
		virtual String    ToString()    const = 0;
		virtual EEventType     GetEventType()const = 0;
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
			if(mEvent.GetEventType() == T::GetStaticType())
			{
				mEvent.Handled |= func(static_cast<T&>(mEvent));
				return true;
			}
			return false;
		}
	};
}