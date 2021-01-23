#pragma once

#include "Window.h"



namespace JG
{
	class WindowsWindow : public IWindow
	{
	private:
		WindowProps mProps;
		HWND mHandle = 0;
		EventCallBackFn mEventFn;
	public:
		virtual ~WindowsWindow() override {}
	public:
		virtual bool Create(const WindowProps& props) override;
		virtual void Update() override;
		virtual void Destroy() override;


		virtual EWindowPlatform GetPlatform() const override;
		virtual ptraddr GetHandle() const override;
	};


}

