#pragma once
#include "GameObject.h"


namespace JG
{
	/* GameComponent
	// GameNode �� ���� Ŭ����
	// ex) MeshRenderer, SpriteRenderer, �� Image, Transform ��� �̷���
	*/

	class GameNode;
	class GameComponent : public GameObject
	{
		friend GameNode;
		GAMECLASS
	private:
		GameNode* mOwnerNode = nullptr;
	protected:
		virtual void Start() override;
		virtual void Destory() override;
	public:

	};
}