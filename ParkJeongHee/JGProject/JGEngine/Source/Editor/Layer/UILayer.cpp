#include "pch.h"
#include "UILayer.h"
#include "Imgui/imgui.h"
#include "UI/UIView/StatisticsView.h"
#include "UI/UIView/SceneView.h"
#include "UI/UIView/WorldHierarchyView.h"

#include "Class/Game/GameWorld.h"
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
		UIManager::GetInstance().RegisterUIView<WorldHierarchyView>();
		LoadUISettings(TT("JGUI.ini"));
	}
	void UILayer::Destroy()
	{
		SaveUISettings(TT("JGUI.ini"));
	}
	void UILayer::OnEvent(IEvent& e)
	{
		//EventDispatcher dispatcher(e);
		//dispatcher.Dispatch<WorldHierarchyUpdateEvent>(EVENT_BIND_FN(&UILayer::OnWorldHierarchyUpdate));
	}
	String UILayer::GetLayerName()
	{
		return TT("UILayer");
	}


	void UILayer::ShowContextMenu(const Type& type)
	{
		if (ImGui::BeginPopupContextItem(ws2s(type.GetName() + TT("_Context")).c_str()))
		{
			UIManager::GetInstance().ForEach(type, UILayer::BeginMenu, UILayer::EndMenu);
			ImGui::EndPopup();
		}
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
			UIManager::GetInstance().ForEach(UILayer::BeginMenu, UILayer::EndMenu);
			ImGui::EndMainMenuBar();
		}

		return EScheduleResult::Continue;
	}

	void UILayer::BeginMenu(const MenuItemNode* Node)
	{
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
	}

	void UILayer::EndMenu(const MenuItemNode* Node)
	{
		if (Node->MenuItem == nullptr && Node->IsOpen)
		{
			ImGui::EndMenu();
		}
	}

}