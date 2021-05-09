#include "GameWorld.h"
#include "GameWorld.h"
#include "pch.h"
#include "GameWorld.h"
#include "Class/Asset/Asset.h"


namespace JG
{
	GameWorld::GameWorld() : GameNode(this)
	{
		mAssetManager = AssetDataBase::GetInstance().RequestAssetManager();
		mUpdateScheduleHandle = ScheduleByFrame(0, 0, -1, 0, [&]() -> EScheduleResult
		{
			Update();
			return EScheduleResult::Continue;
		});
		mLateUpdateScheduleHandle = ScheduleByFrame(0, 0, -1, 0, [&]() -> EScheduleResult
		{
			LateUpdate();
			return EScheduleResult::Continue;
		});

	}
	void GameWorld::Update()
	{
		ForEach([&](GameNode* node) {
			
			if (node->IsActive())
			{
				node->Update();
			}
		});
	}
	void GameWorld::LateUpdate()
	{
		ForEach([&](GameNode* node) {

			if (node->IsActive())
			{
				node->LateUpdate();
			}
		});
	}
	void GameWorld::Destory()
	{
		GameNode::Destory();



		AssetDataBase::GetInstance().ReturnAssetManager(mAssetManager);
		mAssetManager = nullptr;

		mUpdateScheduleHandle->Reset();
		mLateUpdateScheduleHandle->Reset();

		mUpdateScheduleHandle = nullptr;
		mLateUpdateScheduleHandle = nullptr;
	}
	void GameWorld::OnInspectorGUI()
	{
		ImGui::Dummy(ImVec2(0, 1.0f));

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Name "); ImGui::SameLine();
		ImGui::Text(ws2s(GetName()).c_str());
		ImGui::Dummy(ImVec2(0, 1.0f));
		ImGui::Separator();

		for (auto& globalSystem : mGlobalGameSystemList)
		{
		
			ImGui::Spacing();
			String id = globalSystem->GetName() + TT("##") + std::to_wstring((ptraddr)globalSystem);

			if (ImGui::CollapsingHeader(
				ws2s(id).c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader) == true)
			{
				globalSystem->OnInspectorGUI();
			}
		}


		List<GameSystem*> removeSysList;
		for (auto& system : mWorldGameSystemList)
		{
			bool is_open = true;
			ImGui::Spacing();
			String id = system->GetName() + TT("##") + std::to_wstring((ptraddr)system);
			if (ImGui::CollapsingHeader(
				ws2s(id).c_str(), &is_open, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader) == true)
			{
				system->OnInspectorGUI();
			}

			if (is_open == false)
			{
				removeSysList.push_back(system);
			}
		}

		for (auto& sys : removeSysList)
		{
			Destroy(sys);
		}

	}
	AssetManager* GameWorld::GetAssetManager() const
	{

		return mAssetManager.get();
	}
	void GameWorld::AddGameSystem(const Type& type)
	{
		bool isGameSystem = GameObjectFactory::GetInstance().IsGameSystem(type);
		if (isGameSystem == false)
		{
			JG_CORE_ERROR("This Type({0}) is not GameSystem Category.", type.GetName());
			return;
		}
		auto obj = GameObjectFactory::GetInstance().CreateObjectByType(type);
		if (obj == nullptr)
		{
			JG_CORE_ERROR("This Type({0}) is not registered.", type.GetName());
			return;
		}
		auto sys = static_cast<GameSystem*>(obj);
		sys->mGameWorld = this;
		mWorldGameSystemList.push_back(sys);
	}
	void GameWorld::Destroy(GameSystem* sys)
	{
		auto& begin = mWorldGameSystemList.begin();
		auto& end = mWorldGameSystemList.end();
		mWorldGameSystemList.erase(std::remove(begin, end, sys), mWorldGameSystemList.end());
		GameObjectFactory::GetInstance().DestroyObject(sys);
	}
	void GameWorld::SetGlobalGameSystemList(const List<GlobalGameSystem*>& systemList)
	{
		mGlobalGameSystemList = systemList;
	}
	List<GlobalGameSystem*> GameWorld::GetGlobalGameSystemList() const
	{
		return mGlobalGameSystemList;
	}
}