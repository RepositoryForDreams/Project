#pragma once
#include "Class/UI/UIViewModel.h"



namespace JG
{
	class InspectorModel;
	class GameObject;
	class InspectorViewModel : public UIViewModel
	{
		InspectorModel* mInspectorModel = nullptr;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;
	public:
		GameObject* GetTargetGameObject() const;
		void SetTargetGameObject(GameObject* gameObject);
	};
}