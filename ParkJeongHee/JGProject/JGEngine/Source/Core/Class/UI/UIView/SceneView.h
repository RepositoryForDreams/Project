#pragma once
#include "JGCore.h"
#include "Class/UI/UIView.h"
#include "Class/UI/UIViewModel/SceneViewModel.h"

namespace JG
{
	class SceneView : public UIView<SceneViewModel>
	{
		JGCLASS
		bool mOpenGUI = true;


		i32 mCurrentGizmoOperation = 0;
		i32 mCurrentGizmoMode = 0;
		i32 mCurrentCameraMode = 0;
	public:
		SceneView();
		virtual ~SceneView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
	};
}