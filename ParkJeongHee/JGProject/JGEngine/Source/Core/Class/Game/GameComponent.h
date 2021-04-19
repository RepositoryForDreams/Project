#pragma once
#include "GameObject.h"


namespace JG
{
	/* GameComponent
	// GameNode 에 들어가는 클래스
	// ex) MeshRenderer, SpriteRenderer, 뭐 Image, Transform 등등 이런거
	*/

	class GameNode;
	class GameWorld;
	class GameComponent : public GameObject
	{
		friend GameNode;
		GAMECLASS
	private:
		GameNode*  mOwnerNode = nullptr;
		GameWorld* mGameWorld = nullptr;
	protected:
		virtual void Start() override;
		virtual void Destory() override;
	public:
		GameNode*  GetOwner() const;
		GameWorld* GetGameWorld() const;
	};
}