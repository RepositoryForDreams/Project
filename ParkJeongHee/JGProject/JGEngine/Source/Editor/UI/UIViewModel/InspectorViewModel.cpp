#include "pch.h"
#include "InspectorViewModel.h"
#include "UI/UIModel/InspectorModel.h"
namespace JG
{
	void InspectorViewModel::Initialize()
	{
		mInspectorModel = RegisterUIModel<InspectorModel>();

	}
	void InspectorViewModel::Destroy()
	{
		UIViewModel::Destroy();
		mInspectorModel = nullptr;
	}
	GameObject* InspectorViewModel::GetTargetGameObject() const
	{
		if (mInspectorModel != nullptr)
		{
			return mInspectorModel->GetTargetGameObject();
		}
		return nullptr;
	}
	void InspectorViewModel::SetTargetGameObject(GameObject* gameObject)
	{
		if (mInspectorModel != nullptr)
		{
			mInspectorModel->SetTargetGameObject(gameObject);
		}
	}
}