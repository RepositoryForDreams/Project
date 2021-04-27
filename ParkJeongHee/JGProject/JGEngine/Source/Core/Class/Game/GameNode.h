#pragma once
#include "GameObject.h"
#include "GameObjectFactory.h"
namespace JG
{
	class GameComponent;
	class GameWorld;
	class Transform;
	class GameNode : public GameObject
	{
		JGCLASS
	private:
		GameNode*  mParent    = nullptr;
		GameWorld* mGameWorld = nullptr;
		List<GameNode*>      mChilds;
		List<GameComponent*> mComponents;
		Transform* mTransform = nullptr;
	protected:
		virtual void Start() override;
		virtual void Destory() override;
	
	public:
		GameNode();
		GameNode(GameWorld* gameWorld);
		virtual ~GameNode() = default;
	public:
		virtual void OnInspectorGUI() override;
	public:
		GameNode* AddNode(const String& name);

		template<class T>
		T* AddComponent()
		{
			auto obj = GameObjectFactory::GetInstance().CreateObject<T>();
			obj->mOwnerNode = this;
			obj->mGameWorld = mGameWorld;
			mComponents.push_back(obj);
			return obj;
		}
		void AddComponent(const Type& type);
		// 일단 이 클래스의 배열에서 제거하고, 
		void Destroy(GameNode* node);
		void Destroy(GameComponent* component);


		void ForEach(const std::function<void(GameNode*)>& action);
	public:
		GameNode* FindNode(const String& name) const;
		GameNode* FindNode(u32 index) const;


		template<class T>
		T* FindComponent() const
		{
			for (auto& com : mComponents)
			{
				if (com->GetObjectType().GetID() == Type(TypeID<T>()).GetID())
				{
					return static_cast<T*>(com);
				}
			}
			return nullptr;
		}
	public:
		u64 GetChildCount() const;
		virtual void SetParent(GameNode* node);
		GameNode*    GetParent() const;
	private:
		void DestroyRecursive();
	};


}