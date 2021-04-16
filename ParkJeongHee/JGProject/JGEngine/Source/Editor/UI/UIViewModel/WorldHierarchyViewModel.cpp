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


		mAddEmptyObject = CreateUniquePtr<Command<GameNode*>>();
		mAddEmptyObject->Subscribe(this, [&](GameNode* parent)
		{
			parent->AddNode<GameNode>(TT("Empty"));
		});
	}

	void WorldHierarchyViewModel::Destroy()
	{
		UIViewModel::Destroy();

		mAddEmptyObject->UnSubscribe(this);
		mTreeNodePool.clear();
		mAddEmptyObject = nullptr;
		mWorldHierarchyModel = nullptr;
	}

	void WorldHierarchyViewModel::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<ResponseGameWorldEvent>(EVENT_BIND_FN(&WorldHierarchyViewModel::RecieveGameWorldEvent));

		
	}

	void WorldHierarchyViewModel::ForEach(
		const std::function<bool(WorldHierarchyTreeNode&)>& pushAction,
		const std::function<void(WorldHierarchyTreeNode&)>& action,
		const std::function<void(WorldHierarchyTreeNode&)>& popAction)
	{
		if (mWorldHierarchyModel == nullptr)
		{
			return;
		}
		auto gameWorld = mWorldHierarchyModel->GetGameWorld();
		if (gameWorld != nullptr)
		{
			ForEach(gameWorld, pushAction, action, popAction);
		}

	}

	ICommand<GameNode*>* WorldHierarchyViewModel::GetCommand_AddEmptyObject() const
	{
		return mAddEmptyObject.get();
	}

	void WorldHierarchyViewModel::SetCurrentSelectedNode(GameNode* node)
	{
		if (mTreeNodePool.find(node) == mTreeNodePool.end())
		{
			return;
		}
		auto& treeNode = mTreeNodePool[node];
		treeNode.IsSelected = true;

		if (mCurrentSelectedNode != nullptr)
		{
			auto& treeNode = mTreeNodePool[mCurrentSelectedNode];
			treeNode.IsSelected = false;
		}

		mCurrentSelectedNode = node;

	}

	GameNode* WorldHierarchyViewModel::GetCurrentSelectdNode() const
	{
		return mCurrentSelectedNode;
	}


	void WorldHierarchyViewModel::ForEach(GameNode* gameNode,
		const std::function<bool(WorldHierarchyTreeNode&)>& pushAction,
		const std::function<void(WorldHierarchyTreeNode&)>& action,
		const std::function<void(WorldHierarchyTreeNode&)>& popAction)
	{
		if (gameNode == nullptr)
		{
			return;
		}
		auto iter = mTreeNodePool.find(gameNode);
		if (iter == mTreeNodePool.end())
		{
			mTreeNodePool[gameNode].Object = gameNode;
			mTreeNodePool[gameNode].UserFlags = 0;
		}
		auto& treeNode = mTreeNodePool[gameNode];

		bool isOpen = pushAction(treeNode);
		action(treeNode);

		if (treeNode.IsSelected)
		{
			mSelectedNodes.insert(treeNode.Object);
		}
		else
		{
			mSelectedNodes.erase(treeNode.Object);
		}


		if (isOpen == true)
		{
			gameNode->ForEach([&](GameNode* node)
			{
				ForEach(node, pushAction, action, popAction);
			});
		
		}
	
		if (treeNode.IsTreePop)
		{
			popAction(treeNode);
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