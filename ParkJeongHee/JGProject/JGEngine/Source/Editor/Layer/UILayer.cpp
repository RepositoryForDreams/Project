#include "pch.h"
#include "UILayer.h"
#include "Imgui/imgui.h"
#include "UI/UIView/StatisticsView.h"
#include "UI/UIView/SceneView.h"


#include "Class/Game/GameNode.h"
namespace JG
{
	void UILayer::OnAttach()
	{
	
	}
	void UILayer::OnDetach()
	{

	}


	void UILayer::Begin()
	{
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::UILayer, SCHEDULE_BIND_FN(&UILayer::MenuUpdate));


		UIManager::GetInstance().RegisterUIView<SceneView>();
		UIManager::GetInstance().RegisterUIView<StatisticsView>();
		LoadUISettings(TT("JGUI.ini"));
	}
	void UILayer::Destroy()
	{
		SaveUISettings(TT("JGUI.ini"));
	}
	void UILayer::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WorldHierarchyUpdateEvent>(EVENT_BIND_FN(&UILayer::OnWorldHierarchyUpdate));
	}
	String UILayer::GetLayerName()
	{
		return TT("UILayer");
	}

	bool UILayer::OnWorldHierarchyUpdate(WorldHierarchyUpdateEvent& e)
	{
		JG_INFO("Update WorldHierarchy : {0} ", ws2s(e.RootNode->GetName()));
		return true;
	}


	void UILayer::LoadUISettings(const String& fileName)
	{
		Dictionary<String, bool> IsOpen;

		FileStreamReader fileReader;
		if (fileReader.Open(fileName) == true)
		{
			fileReader.Read(&IsOpen);
			fileReader.Close();
		}
		if (IsOpen.empty() == false)
		{
			UIManager::GetInstance().ForEach([&](IUIView* view)
			{

				auto iter = IsOpen.find(view->GetType().GetName());
				if (iter != IsOpen.end())
				{
					if (iter->second)
					{
						view->Open();
					}
					else
					{
						view->Close();
					}
				}
			});
		}

	}
	void UILayer::SaveUISettings(const String& fileName)
	{

		Dictionary<String, bool> IsOpen;
		FileStreamWriter fileWriter;
		if (fileWriter.Open(fileName) == true)
		{
			UIManager::GetInstance().ForEach([&](IUIView* view)
			{
				IsOpen.emplace(view->GetType().GetName(), view->IsOpen());
			});

			fileWriter.Write(IsOpen);
			fileWriter.Close();
		}
		
	}

	EScheduleResult UILayer::MenuUpdate()
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