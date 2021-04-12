﻿#pragma once
#include "JGCore.h"
#include "Platform/Window/Window.h"
#include "Class/LayerStack.h"
#include "Graphics/GraphicsAPI.h"
int main(int argc, char** argv);


namespace JG
{
	struct ApplicaionProps
	{
		EWindowPlatform WindowPlatform;
		WindowProps		WindowProps;
	};
	class Application
	{
	public:
		static const String& GetAssetPath();
	private:
		static Application* smInstance;
	private:
		ApplicaionProps         mProps;
		UniquePtr<IWindow>		mWindow;
		UniquePtr<LayerStack>	mLayerStack;
		UniquePtr<IGraphicsAPI> mGraphcisAPI;

		Queue<UniquePtr<IEvent>> mEventQueue;

		JVector2Uint mDisplaySize;
		bool mIsRunning = true;
		bool mMinimized = false;
	public:
		virtual ~Application();
	private:
		bool Create(const ApplicaionProps& prop);
		void Run();
		void Destroy();
		void OnEvent(IEvent& e);
		bool Open(AppOpenEvent& e);
		bool Resize(AppResizeEvent& e);
		bool Close(AppCloseEvent& e);

	protected:
		virtual void LoadImpl() {}
		virtual void OpenImpl() {}
		virtual void CloseImpl(){}

	protected:
		void PushLayer(const SharedPtr<ILayer>& layer);
		void PushOverlay(const SharedPtr<ILayer>& layer);
	public:
		static Application& GetInstance()
		{
			return *smInstance;
		}
	public:
		IWindow*	  GetWindow() const;
		IGraphicsAPI* GetGraphicsAPI() const;
		JVector2Uint  GetDisplaySize() const;
	public:
		template<class T>
		void SendEvent(const T& e)
		{
			auto pE = CreateUniquePtr<T>();
			*pE = e;
			mEventQueue.push(std::move(pE));
		}
		template<class T>
		void SendEventImmediate(T& e)
		{
			OnEvent(e);
		}
	private:
		friend int ::main(int argc, char** argv);
	};

}
