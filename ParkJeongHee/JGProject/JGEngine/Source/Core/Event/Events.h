#pragma once
#include "Common/Define.h"
#include "Common/Enum.h"
#include "Common/Type.h"
#include "Common/Abstract.h"
#include "Common/Color.h"
#include "Common/Object.h"
#include "Math/Math.h"
#include "Math/JVector.h"
#include "Math/JRect.h"
#include "Math/JRay.h"
#include "Math/JQuaternion.h"
#include "Math/JPlane.h"
#include "Math/JFrustum.h"
#include "Math/JBBox.h"
#include "Math/JMatrix.h"
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

		Request         = 0x100,
		Response        = 0x200,
		Notify			= 0x400,
		Destroy         = 0x800,
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


	// Notify
	class ITexture;
	class NotifyChangeMainSceneTextureEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Notify)
	public:
		SharedPtr<ITexture> SceneTexture;
	public:
		virtual ~NotifyChangeMainSceneTextureEvent() = default;
		virtual String ToString() const override
		{
			return TT("NotifyChangeMainSceneTextureEvent");
		}
	};

	class NotifyChangeGameWorldEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Notify)
	public:
		class GameWorld* GameWorld = nullptr;
	public:
		virtual ~NotifyChangeGameWorldEvent() = default;
		virtual String ToString() const override
		{
			return TT("NotifyChangeGameWorldEvent");
		}
	};


	class NotifyRenderingReadyCompeleteEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Notify)
	public:
		virtual ~NotifyRenderingReadyCompeleteEvent() = default;
		virtual String ToString() const override
		{
			return TT("NotifyRenderingReadyCompeleteEvent");
		}
	};



	//
	// Request Event
	class RequestGetGameWorldEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Request)
	public:
		class GameWorld* GameWorld = nullptr;
	public:
		virtual ~RequestGetGameWorldEvent() = default;
		virtual String ToString() const override
		{
			return TT("RequestGetGameWorldEvent");
		}
	};

	class ITexture;
	class RequestGetMainSceneTextureEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Request)
	public:
		SharedPtr<ITexture> SceneTexture;
	public:
		virtual ~RequestGetMainSceneTextureEvent() = default;
		virtual String ToString() const override
		{
			return TT("RequestGetMainSceneTextureEvent");
		}
	};

	class IRenderItem;
	class RequestPushRenderItemEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Request)
	public:
		SharedPtr<IRenderItem> RenderItem;
	public:
		virtual ~RequestPushRenderItemEvent() = default;
		virtual String ToString() const override
		{
			return TT("PushRenderItemEvent");
		}
	};


	class RequestResolutionResizeEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Request)
	public:
		JVector2 Resolution;
	public:
		virtual ~RequestResolutionResizeEvent() = default;
		virtual String ToString() const override
		{
			return TT("RequestResolutionResizeEvent : X : ") + std::to_wstring(Resolution.x) + TT(", Y : ") + std::to_wstring(Resolution.y);
		}
	};



	class Camera;
	class RequestRegisterCameraEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Request)
	public:
		Camera* SharedCamera;
	public:
		virtual ~RequestRegisterCameraEvent() = default;
		virtual String ToString() const override
		{
			return TT("RequestRegisterCameraEvent");
		}
	};


	class RequestUnRegisterCameraEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Request)
	public:
		Camera* SharedCamera;
	public:
		virtual ~RequestUnRegisterCameraEvent() = default;
		virtual String ToString() const override
		{
			return TT("RequestUnRegisterCameraEvent");
		}
	};











}