#include "pch.h"
#include "StatisticsView.h"
#include "Imgui/imgui.h"
#include "Common/Type.h"


namespace JG
{
	StatisticsView::StatisticsView()
	{
		UIManager::GetInstance().RegisterMenuItem(TT("Windows/StatisticsView"), 0,
			[&]()
		{
			Open();
		}, nullptr);
	}
	void StatisticsView::Initialize()
	{

	}
	void StatisticsView::OnGUI()
	{
		if (ImGui::Begin("StatisticsView", &mOpenGUI))
		{
			auto stats = GetViewModel()->GetRenderer2DStats();

			ImGui::Text("DrawCalls : %d", stats.DrawCalls);
			ImGui::Text("QuadCount : %d", stats.QuadCount);

			ImGui::Text("FPS : %d", Scheduler::GetInstance().GetScheduleTimer()->GetFPS());

			ImGui::End();
		}
		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}
	void StatisticsView::Destroy()
	{



	}
}