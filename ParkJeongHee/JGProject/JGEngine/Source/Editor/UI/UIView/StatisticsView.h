#pragma once
#include "Class/UI/UIView.h"
#include "UI/UIViewModel/StatisticsViewModel.h"


namespace JG
{
	class StatisticsViewModel;
	class StatisticsView : public UIView<StatisticsViewModel>
	{
		UIVIEWCLASS
		bool mOpenGUI = true;
	public:
		StatisticsView();
		virtual ~StatisticsView() = default;
	protected:
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
	};
}