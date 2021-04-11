#pragma once
#include "GameObject.h"
#include "GameObjectFactory.h"
namespace JG
{
	class GameComponent;
	class GameNode : public GameObject
	{
		GameClass(GameNode)
	private:
		GameNode* mParent;
		List<GameNode*>      mChilds;
		List<GameComponent*> mComponents;
	protected:
		virtual void Start() override;
		virtual void Destory() override;
	public:
		virtual ~GameNode() = default;
	public:
		// �߰�
		template<class T>
		T* AddNode(const String& name)
		{
			auto obj = GameObjectFactory::GetInstance().CreateObject<T>();
			obj->SetName(name);
			mChilds.push_back(obj);
			return obj;
		}

		template<class T>
		T* AddComponent()
		{
			auto obj = GameObjectFactory::GetInstance().CreateObject<T>();
			mComponents.push_back(obj);
			return obj;
		}
		// �ϴ� �� Ŭ������ �迭���� �����ϰ�, 
		void Destroy(GameNode* node);
		void Destroy(GameComponent* component);
	public:
		void SetParent(GameNode* node);
		GameNode* GetParent() const;
	private:
		void DestroyRecursive();

	};
}