#include "pch.h"
#include "GameLogicLayer.h"
#include "Application.h"
#include "Class/Game/GameObject.h"

#include "Class/Game/GameNode.h"
#include "Class/Game/GameWorld.h"

#include "Class/Game/GameComponent.h"
#include "Class/Game/Components/Transform.h"

#include "Class/Game/GlobalSystems/SortingLayerSystem.h"


#include "Class/Asset/Asset.h"

namespace JG
{
	void GameLogicLayer::OnAttach()
	{
		GameObjectFactory::Create();
	}
	void GameLogicLayer::OnDetach()
	{
		GameObjectFactory::Destroy();
	}
	void GameLogicLayer::Begin()
	{
		RegisterGameObjectType();
		RegisterGlobalGameSystem();
		mGameWorld = GameObjectFactory::GetInstance().CreateObject<GameWorld>();
		mGameWorld->SetGlobalGameSystemList(mGameSystemList); 
	}
	void GameLogicLayer::Destroy()
	{

	}
	void GameLogicLayer::OnEvent(IEvent& e)
	{
		EventDispatcher eventDispatcher(e);
		eventDispatcher.Dispatch<RequestGameWorldEvent>(EVENT_BIND_FN(&GameLogicLayer::ResponseGameWorld));
	}
	String GameLogicLayer::GetLayerName()
	{
		return TT("GameLogicLayer");
	}
	bool GameLogicLayer::ResponseGameWorld(RequestGameWorldEvent& e)
	{
		ResponseGameWorldEvent response;
		response.GameWorld = mGameWorld;
		Application::GetInstance().SendEvent(response);
		return true;
	}
	void GameLogicLayer::RegisterGlobalGameSystem()
	{
		mGameSystemList.push_back(GameObjectFactory::GetInstance().CreateObject<SortingLayerSystem>());
	}
	void GameLogicLayer::RegisterGameObjectType()
	{
		// GameNode
		GameObjectFactory::GetInstance().RegisterNodeType<GameNode>();
		GameObjectFactory::GetInstance().RegisterNodeType<GameWorld>();

		// GameComponent
		GameObjectFactory::GetInstance().RegisterComponentType<Transform>();

		// GlobalSystem
		GameObjectFactory::GetInstance().RegisterGlobalSystemType<SortingLayerSystem>();
	}
}