#include "pch.h"
#include "UILayer.h"
#include "Imgui/imgui.h"
#include "UI/UIView/StatisticsView.h"
#include "UI/UIView/SceneView.h"
#include "UI/UIView/WorldHierarchyView.h"
#include "UI/UIView/InspectorView.h"
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
		UIManager::GetInstance().RegisterUIView<InspectorView>();


		LoadUISettings(TT("JGUI.ini"));
	}
	void UILayer::Destroy()
	{
		SaveUISettings(TT("JGUI.ini"));
	}
	void UILayer::OnEvent(IEvent& e)
	{

	}
	String UILayer::GetLayerName()
	{
		return TT("UILayer");
	}


	bool UILayer::ShowContextMenu(const Type& type)
	{
		if (ImGui::BeginPopupContextItem())
		{
			UIManager::GetInstance().ForEach(type, UILayer::BeginMenu, UILayer::EndMenu);
			ImGui::EndPopup();
			return true;
		}
		return false;
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
		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGuiWindowFlags   window_flags    = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		window_flags |= ImGuiWindowFlags_NoBackground;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("UILayer", nullptr, window_flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);
		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("UILayer DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			UIManager::GetInstance().ForEach(UILayer::BeginMenu, UILayer::EndMenu);
			ImGui::EndMenuBar();
		}

		ImGui::End();
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