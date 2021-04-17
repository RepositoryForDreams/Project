#include "pch.h"
#include "InspectorModel.h"

namespace JG
{
	void InspectorModel::Initialize()
	{

	}
	void InspectorModel::Destroy()
	{
		SetTargetGameObject(nullptr);
	}
	GameObject* InspectorModel::GetTargetGameObject() const
	{
		return mTargetGameObject;
	}
	void InspectorModel::SetTargetGameObject(GameObject* gameObject)
	{
		mTargetGameObject = gameObject;
	}
}