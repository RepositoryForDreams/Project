#include "pch.h"
#include "WorldHierarchyViewModel.h"
#include "UI/UIModel/WorldHierarchyModel.h"
#include "Application.h"
#include "Class/Game/GameNode.h"
#include "Class/Game/GameWorld.h"


namespace JG
{




	void WorldHierarchyViewModel::Initialize()
	{
		mWorldHierarchyModel = RegisterUIModel<WorldHierarchyModel>();
		RequestGameWorldEvent e;
		SendEvent(e);
	}

	void WorldHierarchyViewModel::Destroy()
	{
		UIViewModel::Destroy();
		mWorldHierarchyModel = nullptr;
	}

	void WorldHierarchyViewModel::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<ResponseGameWorldEvent>(EVENT_BIND_FN(&WorldHierarchyViewModel::RecieveGameWorldEvent));

		
	}

	void WorldHierarchyViewModel::ForEach(std::function<void(GameNode*)> action)
	{
		if (mWorldHierarchyModel == nullptr)
		{
			return;
		}
		if (mWorldHierarchyModel->GetGameWorld() != nullptr)
		{
			action(mWorldHierarchyModel->GetGameWorld());
		}
	
	}



	bool WorldHierarchyViewModel::RecieveGameWorldEvent(ResponseGameWorldEvent& e)
	{
		if (mWorldHierarchyModel != nullptr)
		{
			mWorldHierarchyModel->SetGameWorld(e.GameWorld);
		}
		return true;
	}

}