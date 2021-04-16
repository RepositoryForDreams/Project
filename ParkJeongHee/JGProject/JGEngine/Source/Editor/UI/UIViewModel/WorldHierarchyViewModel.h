#pragma once
#include "Class/UI/UIViewModel.h"




namespace JG
{
	class GameNode;
	struct WorldHierarchyTreeNode
	{
		GameNode* Object = nullptr;
		bool IsSelected  = false;
		bool IsTreePop = true;
		int UserFlags = 0;
	};
	class WorldHierarchyViewModel : public UIViewModel
	{
	private:
		class WorldHierarchyModel* mWorldHierarchyModel = nullptr;
		HashSet<GameNode*> mSelectedNodes;
		GameNode*          mCurrentSelectedNode = nullptr;
	private:
		UniquePtr<Command<GameNode*>> mAddEmptyObject;
	private:
		Dictionary<GameNode*, WorldHierarchyTreeNode> mTreeNodePool;
	public:
		virtual ~WorldHierarchyViewModel() = default;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
	public:
		void ForEach(
			const std::function<bool(WorldHierarchyTreeNode&)>& pushAction,
			const std::function<void(WorldHierarchyTreeNode&)>& action,
			const std::function<void(WorldHierarchyTreeNode&)>& popAction);
		ICommand<GameNode*>* GetCommand_AddEmptyObject() const;

		void SetCurrentSelectedNode(GameNode* node);
		GameNode* GetCurrentSelectdNode() const;
	private:
		void ForEach(
			GameNode* gameNode,
			const std::function<bool(WorldHierarchyTreeNode&)>& pushAction,
			const std::function<void(WorldHierarchyTreeNode&)>& action,
			const std::function<void(WorldHierarchyTreeNode&)>& popAction);
	private:
		bool RecieveGameWorldEvent(ResponseGameWorldEvent& e);
	};
}