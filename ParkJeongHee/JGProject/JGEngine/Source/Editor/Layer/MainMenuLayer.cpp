#include "pch.h"
#include "MainMenuLayer.h"
#include "Imgui/imgui.h"

namespace JG
{
	void MainMenuLayer::OnAttach()
	{

	}
	void MainMenuLayer::OnDetach()
	{

	}

	void MainMenuLayer::Begin()
	{
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::MainMenuLayer, SCHEDULE_BIND_FN(&MainMenuLayer::Update));
	}
	void MainMenuLayer::Destroy()
	{
	}
	void MainMenuLayer::OnEvent(IEvent& e)
	{
	}
	String MainMenuLayer::GetLayerName()
	{
		return TT("MainMenuLayer");
	}
	EScheduleResult MainMenuLayer::Update()
	{
		if (ImGui::BeginMainMenuBar())
		{
			// Menu 
			if (ImGui::BeginMenu("Debug"))
			{


				ImGui::EndMenu();
			}




			ImGui::EndMainMenuBar();
		}

		return EScheduleResult::Continue;
	}
}