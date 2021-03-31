#include "pch.h"
#include "MainMenuLayer.h"
#include "Imgui/imgui.h"

namespace JG
{
	void MainMenuLayer::OnAttach()
	{

	}
	void MainMenuLayer::OnDetach()
	{

	}

	void MainMenuLayer::Begin()
	{
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::MainMenuLayer, SCHEDULE_BIND_FN(&MainMenuLayer::Update));


		UIManager::GetInstance().RegisterMainMenuRootNode(TT("File"),  0);
		UIManager::GetInstance().RegisterMainMenuRootNode(TT("Edit"),  0);
		UIManager::GetInstance().RegisterMainMenuRootNode(TT("Asset"), 0);
		UIManager::GetInstance().RegisterMainMenuRootNode(TT("Window"), UIManager::DEFAULT_PRIORITY + 1);
		UIManager::GetInstance().RegisterMainMenuRootNode(TT("Help"), UIManager::DEFAULT_PRIORITY + 1);


		UIManager::GetInstance().RegisterMainMenuItem(TT("File/Test/Copy"), 0, nullptr, nullptr);
		UIManager::GetInstance().RegisterMainMenuItem(TT("File/Test2/Copy"), 20, nullptr, nullptr);
		UIManager::GetInstance().RegisterMainMenuItem(TT("File/Test3/Copy1"), 11, nullptr, nullptr);
		UIManager::GetInstance().RegisterMainMenuItem(TT("File/Test3/Copy3"), 50, nullptr, nullptr);
		UIManager::GetInstance().RegisterMainMenuItem(TT("File/Test3/Copy2"), 6, nullptr, nullptr);
		UIManager::GetInstance().RegisterMainMenuItem(TT("File/Test/Copy"), 10, nullptr, nullptr);
	}
	void MainMenuLayer::Destroy()
	{

	}
	void MainMenuLayer::OnEvent(IEvent& e)
	{
	}
	String MainMenuLayer::GetLayerName()
	{
		return TT("MainMenuLayer");
	}
	EScheduleResult MainMenuLayer::Update()
	{
		if (ImGui::BeginMainMenuBar())
		{
			const MainMenuItemNode* currParentNode = nullptr;
			u64 currPriority = 0;
			UIManager::GetInstance().ForEach(
				[&](const MainMenuItemNode* Node)
			{

				if (Node->Parent != currParentNode)
				{
					currPriority = Node->Priority;
					currParentNode = Node->Parent;
				}
				if (Node->MenuItem == nullptr)
				{
					if (Node->Parent->IsOpen)
					{
						if (UIManager::GetInstance().IsMainMenuRootNode(Node) == false && Node->Priority - currPriority >= 10)
						{
							ImGui::Separator();
						}
						Node->IsOpen = ImGui::BeginMenu(ws2s(Node->Name).c_str());
					}
					else
					{
						Node->IsOpen = false;
					}
				}
				else
				{
					if (Node->Parent->IsOpen)
					{
						if (UIManager::GetInstance().IsMainMenuRootNode(Node) == false && Node->Priority - currPriority >= 10)
						{
							ImGui::Separator();
						}
						bool isEnable = Node->MenuItem->EnableAction ? Node->MenuItem->EnableAction() : true;
						if (ImGui::MenuItem(ws2s(Node->Name).c_str(), ws2s(Node->MenuItem->ShortCut).c_str(), nullptr, isEnable) == true)
						{
							if (Node->MenuItem->Action)
							{
								Node->MenuItem->Action();
							}
						}
					}
				}

			},
			[&](const MainMenuItemNode* Node)
			{
				if (Node->MenuItem == nullptr && Node->IsOpen)
				{
					ImGui::EndMenu();
				}
			});
			ImGui::EndMainMenuBar();
		}

		
		return EScheduleResult::Continue;
	}
}