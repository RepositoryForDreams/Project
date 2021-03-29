#include "pch.h"
#include "StatisticsViewModel.h"
#include "UI/UIModel/Renderer2DStatisticsModel.h"
namespace JG
{
	bool StatisticsViewModel::Initialize()
	{
		RegisterUIModel<Renderer2DStatisticsModel>();
		return true;
	}
	void StatisticsViewModel::Destroy()
	{
		UIViewModel::Destroy();

	}
	StatisticsViewModel::Renderer2DStatistics StatisticsViewModel::GetRenderer2DStats() const
	{
		auto model = GetUIModel<Renderer2DStatisticsModel>();
		
		return Renderer2DStatistics(model->GetDrawCalls(), model->GetQuadCount());
	}
}