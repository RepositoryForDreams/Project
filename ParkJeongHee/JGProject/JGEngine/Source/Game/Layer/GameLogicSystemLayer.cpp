#include "pch.h"
#include "GameLogicSystemLayer.h"
#include "Application.h"
#include "Class/Game/GameObject.h"

#include "Class/Game/GameNode.h"
#include "Class/Game/GameWorld.h"

#include "Class/Game/GameComponent.h"
#include "Class/Game/Components/Transform.h"
#include "Class/Game/Components/GameCamera.h"

#include "Class/Asset/Asset.h"

namespace JG
{
	void GameLogicSystemLayer::OnAttach()
	{
		GameObjectFactory::Create();
	}
	void GameLogicSystemLayer::OnDetach()
	{
		GameObjectFactory::Destroy();
	}
	void GameLogicSystemLayer::Begin()
	{
		RegisterGameObjectType();
		RegisterGlobalGameSystem();
		mGameWorld = GameObjectFactory::GetInstance().CreateObject<GameWorld>();
		mGameWorld->SetGlobalGameSystemList(mGameSystemList); 
	}
	void GameLogicSystemLayer::Destroy()
	{

	}
	void GameLogicSystemLayer::OnEvent(IEvent& e)
	{
		EventDispatcher eventDispatcher(e);
		eventDispatcher.Dispatch<RequestGetGameWorldEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseGetGameWorld));
	}
	String GameLogicSystemLayer::GetLayerName()
	{
		return TT("GameLogicSystemLayer");
	}
	bool GameLogicSystemLayer::ResponseGetGameWorld(RequestGetGameWorldEvent& e)
	{
		e.GameWorld = mGameWorld;
		return true;
	}
	void GameLogicSystemLayer::RegisterGlobalGameSystem()
	{

	}
	void GameLogicSystemLayer::RegisterGameObjectType()
	{
		// GameNode
		GameObjectFactory::GetInstance().RegisterNodeType<GameNode>();
		GameObjectFactory::GetInstance().RegisterNodeType<GameWorld>();

		// GameComponent
		GameObjectFactory::GetInstance().RegisterComponentType<Transform>();
		GameObjectFactory::GetInstance().RegisterComponentType<GameCamera>();

	}
}