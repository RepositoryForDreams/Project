#include "pch.h"
#include "WorldHierarchyViewModel.h"
#include "UI/UIModel/WorldHierarchyModel.h"
#include "Application.h"
#include "Class/Game/GameNode.h"
#include "Class/Game/GameWorld.h"
#include "UI/UIView/InspectorView.h"


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
		mAddEmptyObject      = nullptr;
		mWorldHierarchyModel = nullptr;
		mCurrentSelectedNodeInInspector   = nullptr;
		mCurrentSelectedNodeInContextMenu = nullptr;
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

	void WorldHierarchyViewModel::SetSelectedNodeInInspector(GameNode* node)
	{
		if (mTreeNodePool.find(node) == mTreeNodePool.end() || node == mCurrentSelectedNodeInInspector)
		{
			return;
		}
		auto& treeNode = mTreeNodePool[node];
		treeNode.IsSelected = true;

		if (mCurrentSelectedNodeInInspector != nullptr)
		{
			auto& treeNode = mTreeNodePool[mCurrentSelectedNodeInInspector];
			treeNode.IsSelected = false;
		}
		if (mCurrentSelectedNodeInContextMenu != nullptr)
		{
			auto& treeNode = mTreeNodePool[mCurrentSelectedNodeInContextMenu];
			treeNode.IsSelected = false;
			mCurrentSelectedNodeInContextMenu = nullptr;
		}

		mCurrentSelectedNodeInInspector = node;

		auto inspector = UIManager::GetInstance().GetUIView<InspectorView>();
		if (inspector != nullptr)
		{
			auto inspectorVm = inspector->GetViewModel();
			if (inspectorVm != nullptr)
			{
				inspectorVm->SetTargetGameObject(mCurrentSelectedNodeInInspector);
			}
		}


	}

	GameNode* WorldHierarchyViewModel::GetSelectedNodeInInspector() const
	{
		return mCurrentSelectedNodeInInspector;
	}

	void WorldHierarchyViewModel::SetSelectedNodeInContextMenu(GameNode* node)
	{
		if (mTreeNodePool.find(node) == mTreeNodePool.end() || node == mCurrentSelectedNodeInContextMenu)
		{
			return;
		}
		auto& treeNode = mTreeNodePool[node];
		treeNode.IsSelected = true;

		if (mCurrentSelectedNodeInContextMenu != nullptr)
		{
			auto& treeNode = mTreeNodePool[mCurrentSelectedNodeInContextMenu];
			treeNode.IsSelected = false;
		}

		if (mCurrentSelectedNodeInInspector != nullptr)
		{
			auto& treeNode = mTreeNodePool[mCurrentSelectedNodeInInspector];
			treeNode.IsSelected = false;
			mCurrentSelectedNodeInInspector = nullptr;
		}
		mCurrentSelectedNodeInContextMenu = node;
	}

	GameNode* WorldHierarchyViewModel::GetSelectdNodeInContextMenu() const
	{
		return mCurrentSelectedNodeInContextMenu;
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