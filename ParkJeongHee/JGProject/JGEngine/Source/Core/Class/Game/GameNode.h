#pragma once
#include "GameObject.h"
#include "GameObjectFactory.h"
namespace JG
{
	class GameComponent;
	class GameNode : public GameObject
	{
		GAMECLASS
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
		// 추가
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
		// 일단 이 클래스의 배열에서 제거하고, 
		void Destroy(GameNode* node);
		void Destroy(GameComponent* component);


		void ForEach(const std::function<void(GameNode*)>& action);
	public:
		GameNode* FindNode(const String& name) const;
		GameNode* FindNode(u32 index) const;

		template<class T>
		T* FindNode() const {
			for (auto& node : mChilds)
			{
				if (node->GetObjectType().GetID() == Type(TypeID<T>()).GetID())
				{
					return node;
				}
			}
			return nullptr;
		}

		template<class T>
		T* FindComponent() const
		{
			for (auto& com : mComponents)
			{
				if (com->GetObjectType().GetID() == Type(TypeID<T>()).GetID())
				{
					return com;
				}
			}
			return nullptr;
		}
	public:
		u64 GetChildCount() const;

		virtual void SetParent(GameNode* node);
		GameNode* GetParent() const;
	private:
		void DestroyRecursive();
	};


}