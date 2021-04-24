#pragma once
#include "GameObject.h"


namespace JG
{
	// 전역 시스템
	class GlobalGameSystem : public GameObject
	{
		GAMECLASS
	protected:
		virtual void Start()   override;
		virtual void Destory() override;
	public:
		virtual void OnInspectorGUI() override;
	};



	// 월드에서만 적용되는 시스템
	class GameSystem : public GameObject
	{
		GAMECLASS

		friend class GameWorld;
		GameWorld* mGameWorld = nullptr;
	protected:
		virtual void Start()   override;
		virtual void Destory() override;
	public:
		virtual void OnInspectorGUI() override;
	};
}