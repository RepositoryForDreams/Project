#include "pch.h"
#include "ImGuiLayer.h"
#include "ExternalImpl/JGImGui.h"

namespace JG
{
	void ImGuiLayer::OnAttach()
	{
		JGImGui::Create();
	}
	void ImGuiLayer::OnDetach()
	{
		JGImGui::Destroy();
	}
	void ImGuiLayer::Update()
	{
		JGImGui::GetInstance().NewFrame();
	}
	void ImGuiLayer::LateUpdate()
	{
	}
	void ImGuiLayer::Begin()
	{
	}
	void ImGuiLayer::Destroy()
	{
	}
	void ImGuiLayer::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<AppResizeEvent>(EVENT_BIND_FN(&ImGuiLayer::Resize));
	}
	String ImGuiLayer::GetLayerName()
	{
		return TT("ImGuiLayer");
	}
	bool ImGuiLayer::Resize(AppResizeEvent& e)
	{
		if (JGImGui::IsValid() == true)
		{
			JGImGui::GetInstance().Resize(e.Width, e.Height);
		}
		return true;
	}
}

