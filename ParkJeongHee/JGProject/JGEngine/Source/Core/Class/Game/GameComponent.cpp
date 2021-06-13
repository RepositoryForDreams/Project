#include "pch.h"
#include "GameComponent.h"



namespace JG
{
	void GameComponent::Start()
	{
		GameObject::Start();
	}
	void GameComponent::Destory()
	{
		GameObject::Destory();
		mOwnerNode = nullptr;
	}
	void GameComponent::Serialize() const
	{
		GameObject::Serialize();
	}
	void GameComponent::DeSerialize()
	{
		GameObject::DeSerialize();
	}
	GameNode* GameComponent::GetOwner() const
	{
		return mOwnerNode;
	}
	GameWorld* GameComponent::GetGameWorld() const
	{
		return mGameWorld;
	}
	bool GameComponent::IsActive() const
	{
		return mIsActive;
	}
}