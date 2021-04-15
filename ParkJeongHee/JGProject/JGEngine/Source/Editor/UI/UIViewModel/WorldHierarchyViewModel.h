#pragma once
#include "Class/UI/UIViewModel.h"




namespace JG
{
	class GameNode;
	class WorldHierarchyViewModel : public UIViewModel
	{
		class WorldHierarchyModel* mWorldHierarchyModel = nullptr;
	public:
		virtual ~WorldHierarchyViewModel() = default;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
	public:
		void ForEach(std::function<void(GameNode*)> action);
	private:
		bool RecieveGameWorldEvent(ResponseGameWorldEvent& e);
	};
}