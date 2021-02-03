#include "EditorLayer.h"
#include "Application.h"
#include "ExternalImpl/ImGui_Impl.h"

namespace JG
{
	void EditorLayer::OnAttach()
	{

	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::Update()
	{
        ImGui::ExternalImpl_BeginFrame();
        static  bool show_demo_window = true;
		static    bool show_another_window = false;
  

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);


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




        ImGui::ExternalImpl_EndFrame(clear_color);
	}

	void EditorLayer::LateUpdate()
	{
	}

	void EditorLayer::Begin()
	{
        ImGui::ExternalImpl_Init(
            Application::GetInstance().GetWindow()->GetPlatform(),
            Application::GetInstance().GetGraphicsAPI()->GetAPI(),
            Application::GetInstance().GetWindow()->GetHandle()
        );
	}

	void EditorLayer::Destroy()
	{
        ImGui::ExternalImpl_Destroy();
	}

	void EditorLayer::OnEvent(IEvent& e)
	{
        EventDispatcher eventDispatcher(e);

        eventDispatcher.Dispatch<AppResizeEvent>(EVENT_BIND_FN(&EditorLayer::Resize));
	}

	String EditorLayer::GetLayerName()
	{
		return TT("JGEditor");
	}

	bool EditorLayer::Resize(AppResizeEvent& e)
	{
        ImGui::ExternalImpl_Resize(e.Width, e.Height);
        return false;
	}
}
