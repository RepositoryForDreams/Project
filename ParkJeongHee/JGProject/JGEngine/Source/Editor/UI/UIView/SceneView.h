#pragma once
#include "JGCore.h"
#include "Class/UI/UIView.h"
#include "UI/UIViewModel/SceneViewModel.h"

namespace JG
{
	class SceneView : public UIView<SceneViewModel>
	{
		bool mOpenGUI = true;
	public:
		SceneView();
	protected:
		virtual bool Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
	public:
		virtual Type GetType() const override;
		virtual ~SceneView() = default;
	};
}