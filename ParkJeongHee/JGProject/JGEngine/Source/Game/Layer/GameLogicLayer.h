#pragma once
#include "JGCore.h"
#include "Class/SystemLayer.h"

namespace JG
{
	class GlobalGameSystem;
	class GameWorld;
	class IGameObject;
	class GameLogicLayer : public ISystemLayer
	{
		GameWorld* mGameWorld = nullptr;
		List<GlobalGameSystem*> mGameSystemList;
	public:
		virtual ~GameLogicLayer() {}
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void Begin() override;
		virtual void Destroy() override;

		virtual void OnEvent(IEvent& e) override;

		virtual String GetLayerName() override;
	public:
		bool ResponseGameWorld(RequestGameWorldEvent& e);
	private:
		void RegisterGlobalGameSystem();
		void RegisterGameObjectType();
	};
}