#include "pch.h"
#include "InspectorViewModel.h"
#include "Class/Game/GameObjectFactory.h"
#include "Class/Game/GameNode.h"
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
	void InspectorViewModel::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<NotifyDestroyGameObjectEvent>(EVENT_BIND_FN(&InspectorViewModel::NotifyDestroyGameObject));
	}
	GameNode* InspectorViewModel::GetTargetGameNode() const
	{
		if (mInspectorModel != nullptr)
		{
			return mInspectorModel->GetTargetGameNode();
		}
		return nullptr;
	}
	void InspectorViewModel::SetTargetGameNode(GameNode* gameNode)
	{
		if (mInspectorModel != nullptr)
		{
			mInspectorModel->SetTargetGameNode(gameNode);
		}
	}
	const SortedSet<String>& InspectorViewModel::FindComponentTypeList(const String& filter)
	{
		if (mInspectorModel != nullptr)
		{
			return mInspectorModel->FindComponentTypeList(filter);
		}
		static SortedSet<String> temp;
		return temp;
	}
	void InspectorViewModel::SelectComponentType(const String& typeName)
	{
		if (GetTargetGameNode() == nullptr)
		{
			return;
		}
		auto gameNode = GetTargetGameNode();
		if (gameNode)
		{
			auto type = GameObjectFactory::GetInstance().GetGameObjectType(typeName);
			gameNode->AddComponent(type);
		}
	}
	bool InspectorViewModel::NotifyDestroyGameObject(NotifyDestroyGameObjectEvent& e)
	{
		auto targetNode = GetTargetGameNode();
		if (targetNode != nullptr)
		{
			if (targetNode->GetID() == e.DestroyedGameObjectID)
			{
				SetTargetGameNode(nullptr);
			}
		}
		return false;
	}
}