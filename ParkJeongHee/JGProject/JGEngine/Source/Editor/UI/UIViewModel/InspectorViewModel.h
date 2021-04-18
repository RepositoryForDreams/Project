#pragma once
#include "Class/UI/UIViewModel.h"



namespace JG
{
	class InspectorModel;
	class GameNode;
	class InspectorViewModel : public UIViewModel
	{
		InspectorModel* mInspectorModel = nullptr;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;
	public:
		GameNode* GetTargetGameNode() const;
		void SetTargetGameNode(GameNode* gameObject);

		const SortedSet<String>& FindComponentTypeList(const String& filter);
		void SelectComponentType(const String& typeName);
	};
}