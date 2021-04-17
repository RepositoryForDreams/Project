#include "pch.h"
#include "StatisticsView.h"
#include "Imgui/imgui.h"
#include "Common/Type.h"


namespace JG
{
	StatisticsView::StatisticsView()
	{
		UIManager::GetInstance().RegisterMainMenuItem(TT("Windows/StatisticsView"), 0,
			[&]()
		{
			Open();
		}, nullptr);
	}
	void StatisticsView::Load()
	{

	}
	void StatisticsView::Initialize()
	{

	}
	void StatisticsView::OnGUI()
	{
		ImGui::Begin("StatisticsView", &mOpenGUI);
		auto stats = GetViewModel()->GetRenderer2DStats();

		ImGui::Text("DrawCalls : %d", stats.DrawCalls);
		ImGui::Text("QuadCount : %d", stats.QuadCount);

		ImGui::Text("FPS : %d", Scheduler::GetInstance().GetScheduleTimer()->GetFPS());

		ImGui::End();

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