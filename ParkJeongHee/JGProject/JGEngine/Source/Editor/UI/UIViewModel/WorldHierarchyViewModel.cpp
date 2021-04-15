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
		mTreeNodePool.clear();
		mWorldHierarchyModel = nullptr;
	}

	void WorldHierarchyViewModel::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<ResponseGameWorldEvent>(EVENT_BIND_FN(&WorldHierarchyViewModel::RecieveGameWorldEvent));

		
	}

	void WorldHierarchyViewModel::ForEach(
		const std::function<bool(WorldHierarchyTreeNode)>& pushAction, 
		const std::function<void(WorldHierarchyTreeNode)>& action,
		const std::function<void(WorldHierarchyTreeNode)>& popAction)
	{
		if (mWorldHierarchyModel == nullptr)
		{
			return;
		}
		auto gameWorld = mWorldHierarchyModel->GetGameWorld();
		if (gameWorld != nullptr)
		{
			auto iter = mTreeNodePool.find(gameWorld);
			if (iter == mTreeNodePool.end())
			{
				mTreeNodePool[gameWorld].Object    = gameWorld;
				mTreeNodePool[gameWorld].UserFlags = 0;
			}
			auto& treeNode = mTreeNodePool[gameWorld];
			
			bool isOpen = pushAction(treeNode);
			action(treeNode);
			if (isOpen == true)
			{
				popAction(treeNode);
			}
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