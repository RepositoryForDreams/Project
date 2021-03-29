#include "pch.h"
#include "StatisticsView.h"
#include "Imgui/imgui.h"
#include "Common/Type.h"


namespace JG
{
	bool StatisticsView::Initialize()
	{
		return true;
	}
	void StatisticsView::OnGUI()
	{
		if (ImGui::Begin("StatisticsView"))
		{
			auto stats = GetViewModel()->GetRenderer2DStats();

			ImGui::Text("DrawCalls : %d", stats.DrawCalls);
			ImGui::Text("QuadCount : %d", stats.QuadCount);


			ImGui::End();
		}

	}
	void StatisticsView::Destroy()
	{



	}
	Type StatisticsView::GetType() const
	{
		return Type(TypeID<StatisticsView>());
	}
}