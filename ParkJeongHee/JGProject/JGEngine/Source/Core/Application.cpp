#include "pch.h"
#include "Application.h"



#include "Platform/Window/WindowsWindow.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
using namespace std;
namespace JG
{
	Application* Application::smInstance = nullptr;

	static String gAssetPath = TT("../Asset/");


	const String& Application::GetAssetPath()
	{
		return gAssetPath;
	}

	Application::~Application() {}

	bool Application::Create(const ApplicaionProps& prop)
	{
		// NOTE
		// 인스턴스 생성
		if(smInstance != nullptr)
		{
			JGASSERT("Already exist application instance");
		}
		smInstance = this;
		mProps = prop;


		
		// TODO
		// 필요한 전역 클래스 생성
		Log::Create();
		TimerManager::Create();
		Scheduler::Create();
		UIManager::Create();

		// TODO
		// 필요한 멤버 클래스 생성
		mLayerStack  = CreateUniquePtr<LayerStack>();
		mGraphcisAPI = IGraphicsAPI::Create(EGraphicsAPI::DirectX12);
		

		// NOTE
		// Window 생성
		switch(prop.WindowPlatform)
		{
		case EWindowPlatform::Windows:
			mWindow = CreateUniquePtr<WindowsWindow>();
		
			break;
		default:
			JGASSERT("Do not supported platform");
			return false;
		}
		mWindow->BindCallBackEventFn(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		
		
		if(!mWindow->Create(prop.WindowProps))
		{
			JG_CORE_CRITICAL("Failed Create Window");
			return false;
		}
		else
		{
			JG_CORE_INFO("Successed Create Window");
			mGraphcisAPI->Create();
			ShaderLibrary::Create();
			Renderer2D::Create();

			
			mIsRunning = true;
			return true;
		}

		return true;
	}
	void Application::Run()
	{
		LoadImpl();
		mLayerStack->ForEach([&](ILayer* layer)
		{
			layer->Begin();
		});
		


		while(mIsRunning)
		{
			if (TimerManager::IsValid())
			{
				TimerManager::GetInstance().Update();
			}
			mGraphcisAPI->Begin();
			mWindow->Update();

			if (Scheduler::IsValid())
			{
				Scheduler::GetInstance().Update();
			}

			mGraphcisAPI->End();
		}

		
		mLayerStack->ForEach([&](ILayer* layer)
		{
			layer->Destroy();
		});
	}

	void Application::Destroy()
	{
		Scheduler::GetInstance().FlushAsyncTask(false);
		mGraphcisAPI->Flush();
		UIManager::Destroy();
		mLayerStack.reset();
		Renderer2D::Destroy();
		ShaderLibrary::Destroy();
		mGraphcisAPI->Destroy();
		mGraphcisAPI.reset();
		mWindow->Destroy();
		mWindow.reset();
		Scheduler::Destroy();
		TimerManager::Destroy();
		Log::Destroy();
	}

	void Application::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<AppOpenEvent>(EVENT_BIND_FN(&Application::Open));
		dispatcher.Dispatch<AppCloseEvent>(EVENT_BIND_FN(&Application::Close));
		dispatcher.Dispatch<AppResizeEvent>(EVENT_BIND_FN(&Application::Resize));


		
		mLayerStack->ForEach([&](ILayer* layer)
		{
			if(e.Handled == false)
			{
				layer->OnEvent(e);
			}
		});
	}
	bool Application::Open(AppOpenEvent& e)
	{
		JG_CORE_INFO(ws2s(e.ToString()));
		OpenImpl();
		return true;
	}
	bool Application::Resize(AppResizeEvent& e)
	{
		if(e.Width == 0 || e.Height == 0)
		{
			mMinimized = true;
			JG_CORE_INFO("최소화 상태");
		}
		else
		{
			mDisplaySize = e.Width;
			mDisplaySize = e.Height;
			mMinimized = false;
		}
		

		return mMinimized;
	}
	bool Application::Close(AppCloseEvent& e)
	{
		JG_CORE_INFO(ws2s(e.ToString()));
		CloseImpl();
		mIsRunning = false;
		return true;
	}

	void Application::PushLayer(const SharedPtr<ILayer>& layer)
	{
		mLayerStack->PushLayer(layer);
	}
	void Application::PushOverlay(const SharedPtr<ILayer>& layer)
	{
		mLayerStack->PushOverlay(layer);
	}

	IWindow* Application::GetWindow() const
	{
		return mWindow.get();
	}
	IGraphicsAPI* Application::GetGraphicsAPI() const
	{
		return mGraphcisAPI.get();
	}
	JVector2Uint Application::GetDisplaySize() const
	{
		return mDisplaySize;
	}
}
