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
		UIManager::GetInstance().RegisterMainMenuItem(TT("File/Save World"), 0, [&]() {
			SaveGameWorld();
		}, nullptr);

		UIManager::GetInstance().RegisterMainMenuItem(TT("File/Load World"), 0, [&]() {
		
			LoadGameWrold();
		}, nullptr);

		RegisterGameObjectType();
		RegisterGlobalGameSystem();
		mGameWorld = GameObjectFactory::GetInstance().CreateObject<GameWorld>();
		mGameWorld->SetGlobalGameSystemList(mGameSystemList); 
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
		GameObjectFactory::GetInstance().RegisterComponentType<SpriteRenderer>();
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
						//auto entry = JGAssetFile::GetEntryJson();
						////mGameWorld->MakeJson(entry);
						//JGAssetFile::Write(savePath, EAssetFormat::GameWorld, entry);
					}
					else
					{
						JG_CORE_ERROR("Failed Save GameWorld : GameWorld is null");
					}


					/*FileStreamWriter writer;
					if (writer.Open(savePath))
					{
						writer.Write(JG_ASSET_FORMAT_KEY, EAssetFormat::GameWorld);
						writer.Write(JG_GAMEWORLD_ASSET_KEY , *mGameWorld);
						writer.Close();
					}*/
				});
			}
			Scheduler::GetInstance().Schedule(1.0f, 0.2f, -1, 0,  [&]()->EScheduleResult
			{
				// 완성했으면 
				if (handle->GetState() == EScheduleState::Compelete)
				{
					auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
					progressBar->Display(TT("Compete"), 1.0f);


					handle->Reset();
					handle = nullptr;

					// 0.5f 뒤에 완료
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
		// 갈아 치우고
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
					//FileStreamReader reader;
					//if (reader.Open(loadPath))
					//{
					//	EAssetFormat assetFormat;
					//	reader.Read(JG_ASSET_FORMAT_KEY , &assetFormat);
					//	if (assetFormat == EAssetFormat::GameWorld)
					//	{
					//		newGameWorld = GameObjectFactory::GetInstance().CreateObject<GameWorld>();
					//		newGameWorld->SetGlobalGameSystemList(mGameSystemList);
					//		is_LoadSucess = true;
					//		reader.Read(JG_GAMEWORLD_ASSET_KEY, newGameWorld);
					//	}
					//	reader.Close();
					//}
				});
			}
			Scheduler::GetInstance().Schedule(1.0f, 0.2f, -1, 0, [&]()->EScheduleResult
			{
				
				// 완성했으면 
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

						// 0.5f 뒤에 완료
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