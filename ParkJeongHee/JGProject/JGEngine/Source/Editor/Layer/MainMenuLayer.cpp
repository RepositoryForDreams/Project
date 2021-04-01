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
			const MenuItemNode* currParentNode = nullptr;
			u64 currPriority = 0;
			UIManager::GetInstance().ForEach(MenuItemNode::ENodeType::MainMenu,
				[&](const MenuItemNode* Node)
			{

				// Node
				if (Node->MenuItem == nullptr)
				{
					if (Node->Parent->IsOpen)
					{
						if (Node->IsSperator == true)
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
				// MenuItem
				else
				{
					if (Node->Parent->IsOpen)
					{
						if (Node->IsSperator == true)
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
			[&](const MenuItemNode* Node)
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