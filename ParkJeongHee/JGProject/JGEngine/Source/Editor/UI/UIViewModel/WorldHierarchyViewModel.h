#pragma once
#include "Class/UI/UIViewModel.h"




namespace JG
{
	class GameNode;
	struct WorldHierarchyTreeNode
	{
		GameNode* Object = nullptr;
		bool IsSelected  = false;
		int UserFlags = 0;
	};
	class WorldHierarchyViewModel : public UIViewModel
	{
		class WorldHierarchyModel* mWorldHierarchyModel = nullptr;
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
			const std::function<bool(WorldHierarchyTreeNode)>& pushAction,
			const std::function<void(WorldHierarchyTreeNode)>& action,
			const std::function<void(WorldHierarchyTreeNode)>& popAction);
	private:
		bool RecieveGameWorldEvent(ResponseGameWorldEvent& e);
	};
}