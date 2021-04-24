#include "pch.h"
#include "GameLayer.h"
#include "Application.h"
#include "Class/Game/GameObject.h"

#include "Class/Game/GameNode.h"
#include "Class/Game/GameWorld.h"

#include "Class/Game/GameComponent.h"
#include "Class/Game/Components/Transform.h"

#include "Class/Asset/Asset.h"

namespace JG
{
	void GameLayer::OnAttach()
	{
		GameObjectFactory::Create();
	}
	void GameLayer::OnDetach()
	{
		GameObjectFactory::Destroy();
	}
	void GameLayer::Begin()
	{
		RegisterGameObjectType();
		mGameWorld = GameObjectFactory::GetInstance().CreateObject<GameWorld>();

		Asset<ITexture> tt;
		IAsset* a = (IAsset*)(&tt);
		auto type = a->GetType();
		JG_INFO("TypeName : {0} , TypeID : {1}", type.GetName(), type.GetID());
	}
	void GameLayer::Destroy()
	{

	}
	void GameLayer::OnEvent(IEvent& e)
	{
		EventDispatcher eventDispatcher(e);
		eventDispatcher.Dispatch<RequestGameWorldEvent>(EVENT_BIND_FN(&GameLayer::ResponseGameWorld));
	}
	String GameLayer::GetLayerName()
	{
		return TT("GameLayer");
	}
	bool GameLayer::ResponseGameWorld(RequestGameWorldEvent& e)
	{
		ResponseGameWorldEvent response;
		response.GameWorld = mGameWorld;
		Application::GetInstance().SendEvent(response);
		return true;
	}
	void GameLayer::RegisterGameObjectType()
	{
		// GameNode
		GameObjectFactory::GetInstance().RegisterNodeType<GameNode>();
		GameObjectFactory::GetInstance().RegisterNodeType<GameWorld>();

		//// GameComponent
		GameObjectFactory::GetInstance().RegisterComponentType<GameComponent>();
		GameObjectFactory::GetInstance().RegisterComponentType<Transform>();
	}
}