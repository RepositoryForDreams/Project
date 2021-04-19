#pragma once
#include "GameObject.h"


namespace JG
{
	/* GameComponent
	// GameNode �� ���� Ŭ����
	// ex) MeshRenderer, SpriteRenderer, �� Image, Transform ��� �̷���
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