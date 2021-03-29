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
	void ImGuiLayer::Begin() 
	{
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::ImGuiLayer, SCHEDULE_BIND_FN(&ImGuiLayer::Update));
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


	EScheduleResult ImGuiLayer::Update()
	{
		JGImGui::GetInstance().NewFrame();

		return EScheduleResult::Continue;
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

