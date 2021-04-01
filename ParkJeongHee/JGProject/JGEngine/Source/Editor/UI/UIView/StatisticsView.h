#pragma once
#include "Class/UI/UIView.h"
#include "UI/UIViewModel/StatisticsViewModel.h"


namespace JG
{
	class StatisticsViewModel;
	class StatisticsView : public UIView<StatisticsViewModel>
	{
		bool mOpenGUI = true;
	public:
		StatisticsView();
	protected:
		virtual bool Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
	public:
		virtual Type GetType() const override;
		virtual ~StatisticsView() = default;
	};
}