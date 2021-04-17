#pragma once

#include "Class/UI/UIModel.h"


namespace JG
{
	class InspectorModel : public UIModel
	{
		class GameObject* mTargetGameObject = nullptr;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;
	public:
		GameObject* GetTargetGameObject() const;
		void SetTargetGameObject(GameObject* gameObject);
	};
}