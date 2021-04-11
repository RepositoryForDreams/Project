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
}