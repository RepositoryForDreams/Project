#pragma once

#include "Class/UI/UIModel.h"


namespace JG
{
	class GameNode;
	class InspectorModel : public UIModel
	{
		GameNode* mTargetGameNode = nullptr;
		String            mFilterStr;
		HashSet<Type>     mComponentTypeSet;
		SortedSet<String> mFilteredTypeList;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;
	public:
		GameNode* GetTargetGameNode() const;
		void SetTargetGameNode(GameNode* gameObject);
		const SortedSet<String>& FindComponentTypeList(const String& filter);
	};
}