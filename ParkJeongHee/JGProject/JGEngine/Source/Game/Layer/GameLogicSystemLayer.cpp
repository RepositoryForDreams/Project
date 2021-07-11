#include "pch.h"
#include "GameLogicSystemLayer.h"
#include "Application.h"
#include "Class/Game/GameObject.h"

#include "Class/Game/GameNode.h"
#include "Class/Game/GameWorld.h"

#include "Class/Game/GameComponent.h"
#include "Class/Game/Components/Transform.h"
#include "Class/Game/Components/Camera.h"
#include "Class/Game/Components/SpriteRenderer.h"
#include "Class/Game/Components/StaticMeshRenderer.h"

#include "Class/Asset/Asset.h"

#include "Class/UI/ModalUI/ProgressBarModalView.h"
namespace JG
{
	void GameLogicSystemLayer::OnAttach()
	{
		GameObjectFactory::Create();
	}
	void GameLogicSystemLayer::OnDetach()
	{
		GameObjectFactory::Destroy();
	}
	void GameLogicSystemLayer::Begin()
	{
		static bool testopen = false;
		UIManager::GetInstance().RegisterMainMenuItem(TT("File/Save World %_S"), 0, [&]() {
			SaveGameWorld();
		}, nullptr);

		UIManager::GetInstance().RegisterMainMenuItem(TT("File/Load World"), 0, [&]() {
		
			LoadGameWrold();
		}, nullptr);

		RegisterGameObjectType();
		RegisterGlobalGameSystem();
		mGameWorld = GameObjectFactory::GetInstance().CreateObject<GameWorld>();
		mGameWorld->SetGlobalGameSystemList(mGameSystemList); 
		auto camNode = mGameWorld->AddNode(TT("EditorCamera"));
		auto editorCam = camNode->AddComponent<EditorCamera>();
		Camera::SetMainCamera(editorCam);
		LoadGameWrold();
	}
	void GameLogicSystemLayer::Destroy()
	{

	}
	void GameLogicSystemLayer::OnEvent(IEvent& e)
	{
		EventDispatcher eventDispatcher(e);
		eventDispatcher.Dispatch<RequestGetGameWorldEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseGetGameWorld));
		eventDispatcher.Dispatch<NotifyRenderingReadyCompeleteEvent>(EVENT_BIND_FN(&GameLogicSystemLayer::ResponseNotfyRenderingReadyCompelete));
	}
	String GameLogicSystemLayer::GetLayerName()
	{
		return TT("GameLogicSystemLayer");
	}
	bool GameLogicSystemLayer::ResponseGetGameWorld(RequestGetGameWorldEvent& e)
	{
		e.GameWorld = mGameWorld;
		return true;
	}
	bool GameLogicSystemLayer::ResponseNotfyRenderingReadyCompelete(NotifyRenderingReadyCompeleteEvent& e)
	{
		if (mGameWorld != nullptr)
		{
			auto item = mGameWorld->PushRenderItem();
			if (item != nullptr)
			{
				RequestPushRenderItemEvent e;
				e.RenderItem = item;
				Application::GetInstance().SendEvent(e);
			}
		}
		return true;
	}
	bool GameLogicSystemLayer::ResponseEditorSceneOnClickEvent(NotifyEditorSceneOnClickEvent& e)
	{
		if (mGameWorld != nullptr)
		{




		}
		return true;
	}
	void GameLogicSystemLayer::RegisterGlobalGameSystem()
	{



	}
	void GameLogicSystemLayer::RegisterGameObjectType()
	{
		// GameNode
		GameObjectFactory::GetInstance().RegisterNodeType<GameNode>();
		GameObjectFactory::GetInstance().RegisterNodeType<GameWorld>();

		// GameComponent
		GameObjectFactory::GetInstance().RegisterComponentType<Transform>();
		GameObjectFactory::GetInstance().RegisterComponentType<Camera>();
		GameObjectFactory::GetInstance().RegisterComponentType<EditorCamera>();
		GameObjectFactory::GetInstance().RegisterComponentType<SpriteRenderer>();
		GameObjectFactory::GetInstance().RegisterComponentType<StaticMeshRenderer>();
	}
	void GameLogicSystemLayer::SaveGameWorld()
	{
		if (mGameWorld == nullptr)
		{
			return;
		}
		static SharedPtr<ScheduleHandle> handle = nullptr;
		Scheduler::GetInstance().ScheduleOnce(0.0f, 0, [&]() -> EScheduleResult
		{
			auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
			if (progressBar->IsOpen() == false)
			{
				UIManager::GetInstance().OpenPopupUIView<ProgressBarModalView>(ProgressBarInitData(TT("Save World")));
				progressBar->Display(TT("Save.. "), 0.5f);

				handle = Scheduler::GetInstance().ScheduleAsync([&](void* data)
				{
					auto savePath = CombinePath(Application::GetAssetPath(), TT("testGameWorld")) + JG_ASSET_FORMAT;
				
					if (mGameWorld != nullptr)
					{
						auto json = CreateSharedPtr<Json>();
						json->AddMember(JG_ASSET_FORMAT_KEY, (u64)EAssetFormat::GameWorld);
						auto assetJson = json->CreateJsonData();
						mGameWorld->MakeJson(assetJson);
						json->AddMember(JG_ASSET_KEY, assetJson);
						Json::Write(savePath, json);
					}
					else
					{
						JG_CORE_ERROR("Failed Save GameWorld : GameWorld is null");
					}
				});
			}
			Scheduler::GetInstance().Schedule(1.0f, 0.2f, -1, 0,  [&]()->EScheduleResult
			{
				// �ϼ������� 
				if (handle->GetState() == EScheduleState::Compelete)
				{
					auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
					progressBar->Display(TT("Compete"), 1.0f);


					handle->Reset();
					handle = nullptr;

					// 0.5f �ڿ� �Ϸ�
					Scheduler::GetInstance().ScheduleOnce(0.5f, 0, [&]()->EScheduleResult
					{
						auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
						progressBar->Close();
						JG_CORE_INFO("Comepete Save GameWorld");
						return EScheduleResult::Break;
					});
					return EScheduleResult::Break;
				}

	

				return EScheduleResult::Continue;
			});

			return EScheduleResult::Continue;
		});
	}
	void GameLogicSystemLayer::LoadGameWrold()
	{
		static GameWorld* newGameWorld = nullptr;
		static GameWorld* oldGameWorld = nullptr;
		static bool is_LoadSucess = false;
		// ���� ġ���
		// 
		static SharedPtr<ScheduleHandle> handle = nullptr;
		Scheduler::GetInstance().ScheduleOnce(0.0f, 0, [&]() -> EScheduleResult
		{
			auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
			
			if (progressBar->IsOpen() == false)
			{
				UIManager::GetInstance().OpenPopupUIView<ProgressBarModalView>(ProgressBarInitData(TT("Load World")));
				progressBar->Display(TT("Load.. "), 0.5f);

				handle = Scheduler::GetInstance().ScheduleAsync([&](void* data)
				{
					auto loadPath = CombinePath(Application::GetAssetPath(), TT("testGameWorld")) + JG_ASSET_FORMAT;
					auto json = CreateSharedPtr<Json>();
					if (Json::Read(loadPath, json) == true)
					{
						EAssetFormat assetFormat = EAssetFormat::None;
						auto assetFormatVal = json->GetMember(JG_ASSET_FORMAT_KEY);
						if (assetFormatVal)
						{
							assetFormat = (EAssetFormat)assetFormatVal->GetUint64();
						}

						if (EAssetFormat::GameWorld == assetFormat)
						{
							auto assetVal = json->GetMember(JG_ASSET_KEY);
							if (assetVal)
							{
								newGameWorld = GameObjectFactory::GetInstance().CreateObject<GameWorld>();
								newGameWorld->SetGlobalGameSystemList(mGameSystemList);
								newGameWorld->LoadJson(assetVal);
								is_LoadSucess = true;
							}
						}
					}
				});
			}
			Scheduler::GetInstance().Schedule(1.0f, 0.2f, -1, 0, [&]()->EScheduleResult
			{
				
				// �ϼ������� 
				if (handle->GetState() == EScheduleState::Compelete)
				{
					if (is_LoadSucess)
					{
						auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
						progressBar->Display(TT("Compete"), 1.0f);

						oldGameWorld = mGameWorld;
						mGameWorld = newGameWorld;
						NotifyChangeGameWorldEvent e;
						e.GameWorld = mGameWorld;
						Application::GetInstance().SendEvent(e);

						handle->Reset();
						handle = nullptr;
						newGameWorld = nullptr;

						// 0.5f �ڿ� �Ϸ�
						Scheduler::GetInstance().ScheduleOnce(0.5f, 0, [&]()->EScheduleResult
						{
							auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
							progressBar->Close();
							JG_CORE_INFO("Comepete Save GameWorld");

							return EScheduleResult::Break;
						});
						Scheduler::GetInstance().ScheduleOnceByFrame(1, 0, [&]() ->EScheduleResult
						{
							if (oldGameWorld)
							{
								oldGameWorld->Destroy(oldGameWorld);
							}
							return EScheduleResult::Break;
						});
					}
					else
					{
						auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
						progressBar->Display(TT("Failed"), 1.0f);
						Scheduler::GetInstance().ScheduleOnce(0.5f, 0, [&]()->EScheduleResult
						{
							auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
							progressBar->Close();
							JG_CORE_INFO("Comepete Load GameWorld");

							return EScheduleResult::Break;
						});
					}



					return EScheduleResult::Break;
				}



				return EScheduleResult::Continue;
			});
			return EScheduleResult::Continue;
		});



	}
}