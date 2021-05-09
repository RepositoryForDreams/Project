#pragma once
#include "Class/UI/UIView.h"
#include "UI/UIViewModel/StatisticsViewModel.h"


namespace JG
{
	class StatisticsViewModel;
	class StatisticsView : public UIView<StatisticsViewModel>
	{
		JGCLASS
		bool mOpenGUI = true;
	public:
		StatisticsView();
		virtual ~StatisticsView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
	};
}