#include "pch.h"
#include "GameNode.h"
#include "GameComponent.h"

namespace JG
{
	void GameNode::Start()
	{
		GameObject::Start();
	}
	void GameNode::Destory()
	{
		GameObject::Destory();
		SetParent(nullptr);
	}
	void GameNode::Destroy(GameNode* node)
	{
		node->DestroyRecursive();
	}
	void GameNode::Destroy(GameComponent* component)
	{
		auto& begin = mComponents.begin();
		auto& end   = mComponents.end();
		mComponents.erase(std::remove(begin, end, component), mComponents.end());
		GameObjectFactory::GetInstance().DestroyObject(component);
	}
	void GameNode::ForEach(const std::function<void(GameNode*)>& action)
	{
		for (auto& node : mChilds)
		{
			action(node);
		}
	}
	GameNode* GameNode::FindNode(const String& name) const
	{
		for (auto& node : mChilds)
		{
			if (node->GetName() == name)
			{
				return node;
			}
		}
		return nullptr;
	}
	GameNode* GameNode::FindNode(u32 index) const
	{
		if (mChilds.size() <= index)
		{
			return nullptr;
		}
		return mChilds[index];
	}
	void GameNode::SetParent(GameNode* node)
	{
		bool isDiff = mParent != node;
		if (isDiff == false)
		{
			return;
		}
		if (mParent != nullptr)
		{
			auto& begin = mParent->mChilds.begin();
			auto& end   = mParent->mChilds.end();
			mParent->mChilds.erase(std::remove(begin, end, node), mParent->mChilds.end());
			mParent = nullptr;
		}
		if (node != nullptr)
		{
			mParent = node;
			mParent->mChilds.push_back(this);
		}
	}
	GameNode* GameNode::GetParent() const
	{
		return mParent;
	}
	void GameNode::DestroyRecursive()
	{
		for (auto& child : mChilds)
		{
			child->DestroyRecursive();
		}
		for (auto& com : mComponents)
		{
			GameObjectFactory::GetInstance().DestroyObject(com);
		}
		for (auto& child : mChilds)
		{
			GameObjectFactory::GetInstance().DestroyObject(child);
		}

	}
}