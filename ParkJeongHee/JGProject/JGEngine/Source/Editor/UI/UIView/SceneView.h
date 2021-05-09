#pragma once
#include "JGCore.h"
#include "Class/UI/UIView.h"
#include "UI/UIViewModel/SceneViewModel.h"

namespace JG
{
	class SceneView : public UIView<SceneViewModel>
	{
		JGCLASS
		bool mOpenGUI = true;
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