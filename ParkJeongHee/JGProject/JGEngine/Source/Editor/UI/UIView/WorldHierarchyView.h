#pragma once


#include "Class/UI/UIView.h"
#include "UI/UIViewModel/WorldHierarchyViewModel.h"

namespace JG
{
	class WorldHierarchyView : public UIView<WorldHierarchyViewModel>
	{
		UIVIEWCLASS

		bool mOpenGUI = true;
		WorldHierarchyViewModel* mVm = nullptr;
	public:
		WorldHierarchyView();
	public:
		virtual ~WorldHierarchyView() = default;
	protected:
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;

	};

}