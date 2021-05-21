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
	void GameComponent::Serialize(FileStreamWriter* writer) const
	{
		GameObject::Serialize(writer);
		writer->Write(mIsActive);
	}
	void GameComponent::DeSerialize(FileStreamReader* reader)
	{
		GameObject::DeSerialize(reader);
		reader->Read(&mIsActive);
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