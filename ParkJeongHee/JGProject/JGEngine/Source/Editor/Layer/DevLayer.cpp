#include "DevLayer.h"
#include "Application.h"
#include "Imgui/imgui.h"
#include "ExternalImpl/JGImGui.h"
#include "Platform/Window/Window.h"
#include "Graphics/GraphicsAPI.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Camera.h"
#include "Graphics/Resource.h"
#include "Class/UI/UIView.h"
#include "UI/UIView/StatisticsView.h"
using namespace std;
namespace JG
{
	void DevLayer::OnAttach()
	{

	}
	void DevLayer::OnDetach()
	{

	}
	//void DevLayer::Update()
	//{

		//static  bool show_demo_window = true;
		//static    bool show_another_window = false;


		//if (Renderer2D::Begin(mCamera))
		//{

		//	auto r = Color::Red(); r.A = 0.8f;
		//	Renderer2D::DrawCall(
		//		JVector2(0,0), JVector2(200, 200), r
		//	);

		//	Renderer2D::DrawCall(
		//		JVector2(400, 0), JVector2(200, 200), Color::Blue()
		//	);

		//	Renderer2D::End();

		//}
		//

		//ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		//// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		//{

		//	static float f = 0.0f;
		//	static int counter = 0;

		//	if (ImGui::Begin("Hello, world!"))
		//	{
		//		ImGui::Begin("TestChile");
		//		ImGui::End();
		//		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		//		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		//		ImGui::Checkbox("Another Window", &show_another_window);

		//		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		//		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		//			counter++;
		//		ImGui::SameLine();
		//		ImGui::Text("counter = %d", counter);

		//		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//		ImGui::Image((ImTextureID)JGImGui::GetInstance().ConvertImGuiTextureID(mCamera->GetTargetTexture()->GetTextureID()), { 960, 540 });
		//		ImGui::End();
		//	}

		//	
		//}

		//// 3. Show another simple window.
		//if (show_another_window)
		//{
		//	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		//	ImGui::Text("Hello from another window!");
		//	if (ImGui::Button("Close Me"))
		//		show_another_window = false;
		//	ImGui::End();
		//}
		//static bool is_show = true;
		//if (is_show)
		//{
		//	if (ImGui::Begin("Statistics"), &is_show)
		//	{
		//		auto stats = Renderer2D::GetStats();

		//		std::string drawCall = "DrawCall : " + to_string(stats.DrawCalls);
		//		std::string qaudCount = "QuadCount : " + to_string(stats.QuadCount);
		//		std::string vertexCount = "VertexCount : " + to_string(stats.GetTotalVertexCount());
		//		std::string indexCount = "IndexCount : " + to_string(stats.GetTotalIndexCount());
		//		ImGui::Text(drawCall.c_str());
		//		ImGui::Text(qaudCount.c_str());
		//		ImGui::Text(vertexCount.c_str());
		//		ImGui::Text(indexCount.c_str());
		//		ImGui::End();
		//	}
		//}
	//}
	void DevLayer::Begin()
	{
		List<int> s;
		
		mCamera = Camera::Create(JVector2(1920, 1080), 60, 0.1f, 1000.0f, true);
		mCamera->SetLocation(JVector3(0, 0, -10));
		TextureInfo textureInfo;

		textureInfo.Width  = 1920;
		textureInfo.Height = 1080;
		textureInfo.ClearColor = Color::Green();
		textureInfo.Format = ETextureFormat::R8G8B8A8_Unorm;
		textureInfo.MipLevel = 1;
		textureInfo.Flags = ETextureFlags::Allow_RenderTarget;
		textureInfo.ArraySize = 1;
		auto texture = ITexture::Create(TT("DevLayer_Texture"), textureInfo);
		mCamera->SetTargetTexture(texture);
		static int test = 0;
		Scheduler::GetInstance().ScheduleOnce(0, 0, [&]()->EScheduleResult
		{
			JG_CORE_INFO("Stack {0}", test++);
			mTest.SetValue(test);
			Scheduler::GetInstance().ScheduleOnce(0, 0, [&]()->EScheduleResult
			{
				JG_CORE_INFO("Stack {0}", test++);
				mTest.SetValue(test);
				Scheduler::GetInstance().ScheduleOnce(0, 0, [&]()->EScheduleResult
				{
					JG_CORE_INFO("Stack {0}", test++);
					mTest.SetValue(test);
					Scheduler::GetInstance().ScheduleOnce(0, 0, [&]()->EScheduleResult
					{
						JG_CORE_INFO("Stack {0}", test++);
						mTest.SetValue(test);
						Scheduler::GetInstance().ScheduleOnce(0, 0, [&]()->EScheduleResult
						{
							JG_CORE_INFO("Stack {0}", test++);
							mTest.SetValue(test);
							for (int i = 0; i < 10; ++i)
							{
								Scheduler::GetInstance().ScheduleAsync([&]()
								{
									std::hash<std::thread::id> hasher;
									JG_CORE_INFO("Thread ID : {0} ", hasher(std::this_thread::get_id()));
									mTest.SetValue(hasher(std::this_thread::get_id()));
								});
							}
					
							return EScheduleResult::Continue;
						});
						return EScheduleResult::Continue;
					});
					return EScheduleResult::Continue;
				});
				return EScheduleResult::Continue;
			});
			return EScheduleResult::Continue;
		});

		mTest.Subscribe(this,
			[&](u64 a)
		{
			JG_CORE_INFO("A {0}", a);

		});
		Scheduler::GetInstance().ScheduleOnce(0, 0, [&]()->EScheduleResult
		{
			auto view = UIManager::GetInstance().GetUIView<StatisticsView>();
			if (view->IsOpen() == false)
			{
				view->Open();
			}
			return EScheduleResult::Continue;
		});

	}
	void DevLayer::Destroy()
	{
		mTest.UnSubscribe(this);
	}
	void DevLayer::OnEvent(IEvent& e)
	{

	}
	String DevLayer::GetLayerName()
	{
		return TT("DevLayer");
	}
}