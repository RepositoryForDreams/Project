#pragma once
#include "JGCore.h"
#include "Class/UI/UIView.h"
#include "UI/UIViewModel/SceneViewModel.h"

namespace JG
{
	class SceneView : public UIView<SceneViewModel>
	{
		UIVIEWCLASS
		bool mOpenGUI = true;
	public:
		SceneView();
		virtual ~SceneView() = default;
	protected:
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
	};
}