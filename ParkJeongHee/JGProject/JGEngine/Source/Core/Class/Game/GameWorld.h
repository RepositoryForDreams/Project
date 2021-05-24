#pragma once
#include "GameNode.h"
#include "GameSystem.h"
#include "Class/Asset/AssetManager.h"
namespace JG
{
	class GameWorld : public GameNode
	{
		JGCLASS
	private:
		SharedPtr<AssetManager> mAssetManager;
		List<GameSystem*>	    mWorldGameSystemList;
		List<GlobalGameSystem*> mGlobalGameSystemList;

		SharedPtr<ScheduleHandle> mUpdateScheduleHandle;
		SharedPtr<ScheduleHandle> mLateUpdateScheduleHandle;
	public:
		GameWorld();
	protected:
		virtual void Update() override;
		virtual void LateUpdate() override;
		virtual void Destory() override;
	protected:
		virtual void Serialize(FileStreamWriter* writer)   const override;
		virtual void DeSerialize(FileStreamReader* reader) override;
	public:
		virtual void SetParent(GameNode* node) override {}
		virtual void OnInspectorGUI() override;
		
	public:
		AssetManager* GetAssetManager() const;
	public:
		template<class T>
		T* AddGameSystem()
		{
			auto obj = GameObjectFactory::GetInstance().CreateObject<T>();
			obj->mGameWorld = this;
			mWorldGameSystemList.push_back(obj);
			return obj;
		}
		void AddGameSystem(const Type& type);
		void Destroy(GameSystem* sys);
		void Destroy(GameWorld* world);
	private:
		friend class GameLogicSystemLayer;
		void SetGlobalGameSystemList(const List<GlobalGameSystem*>& systemList);
		List<GlobalGameSystem*> GetGlobalGameSystemList() const;
	};
}