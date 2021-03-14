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
using namespace std;
namespace JG
{
	void DevLayer::OnAttach()
	{
		JGImGui::Create();
	}
	void DevLayer::OnDetach()
	{
		JGImGui::Destroy();
	}
	void DevLayer::Update()
	{
		JGImGui::GetInstance().NewFrame();
		static  bool show_demo_window = true;
		static    bool show_another_window = false;


		if (Renderer2D::Begin(mCamera, mRenderTexture))
		{

			auto r = Color::Red(); r.A = 0.8f;
			Renderer2D::DrawCall(
				JVector2(0,0), JVector2(200, 200), r
			);

			Renderer2D::DrawCall(
				JVector2(400, 0), JVector2(200, 200), Color::Blue()
			);

			Renderer2D::End();

		}

		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Image((ImTextureID)JGImGui::GetInstance().ConvertImGuiTextureID(mRenderTexture->GetTextureID()), { 1920, 1080 });
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		


	}
	void DevLayer::LateUpdate()
	{

	}
	void DevLayer::Begin()
	{

		auto AppSize = Application::GetInstance().GetSize();
		mCamera = Camera::Create(JVector2(1920, 1080), 60, 0.1f, 1000.0f, true);
		mCamera->SetLocation(JVector3(0, 0, -10));
		TextureInfo textureInfo;

		textureInfo.Width = 1920;
		textureInfo.Height = 1080;
		textureInfo.ClearColor = Color::Green();
		textureInfo.Format = ETextureFormat::R8G8B8A8_Unorm;
		textureInfo.MipLevel = 1;
		textureInfo.Flags = ETextureFlags::Allow_RenderTarget;
		textureInfo.ArraySize = 1;
		mRenderTexture = ITexture::Create(TT("DevLayer_Texture"), textureInfo);
	}
	void DevLayer::Destroy()
	{
	
	}
	void DevLayer::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<AppResizeEvent>(EVENT_BIND_FN(&DevLayer::Resize));
	}
	String DevLayer::GetLayerName()
	{
		return TT("DevLayer");
	}
	bool DevLayer::Resize(AppResizeEvent& e)
	{
		if (JGImGui::IsValid() == true)
		{
			JGImGui::GetInstance().Resize(e.Width, e.Height);
		}
		return true;
	}
}