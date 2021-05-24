#include "pch.h"
#include "UISystemLayer.h"
#include "Imgui/imgui.h"

// UI
#include "Class/UI/UIView/SceneView.h"
#include "Class/UI/UIView/WorldHierarchyView.h"
#include "Class/UI/UIView/InspectorView.h"
#include "Class/UI/UIView/ContentsView.h"

// PopupUI
#include "Class/UI/ContextUI/ComponentFinderContextView.h"
#include "Class/UI/ContextUI/AssetFinderContextView.h"
//
#include "Class/UI/ModalUI/ProgressBarModalView.h"

#include "Class/Game/GameWorld.h"
namespace JG
{
	void UISystemLayer::OnAttach()
	{
	
	}
	void UISystemLayer::OnDetach()
	{

	}


	void UISystemLayer::Begin()
	{
		
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::UISystemLayer, SCHEDULE_BIND_FN(&UISystemLayer::MenuUpdate));
		UIManager::GetInstance().BindShowContextMenuFunc([&](Type type) -> bool {
			if (ImGui::BeginPopupContextItem())
			{
				UIManager::GetInstance().ForEach(type, UISystemLayer::BeginMenu, UISystemLayer::EndMenu);
				ImGui::EndPopup();
				return true;
			}
			return false;
		});

		// UI
		UIManager::GetInstance().RegisterUIView<SceneView>();
		UIManager::GetInstance().RegisterUIView<WorldHierarchyView>();
		UIManager::GetInstance().RegisterUIView<InspectorView>();
		UIManager::GetInstance().RegisterUIView<ContentsView>();

		// PopupUI
		UIManager::GetInstance().RegisterPopupUIView<ComponentFinderContextView>();
		UIManager::GetInstance().RegisterPopupUIView<AssetFinderContextView>();
		// Modal
		UIManager::GetInstance().RegisterPopupUIView<ProgressBarModalView>();
		LoadUISettings(TT("JGUI.ini"));
	}
	void UISystemLayer::Destroy()
	{
		SaveUISettings(TT("JGUI.ini"));
	}
	void UISystemLayer::OnEvent(IEvent& e)
	{

	}
	String UISystemLayer::GetLayerName()
	{
		return TT("UISystemLayer");
	}

	void UISystemLayer::LoadUISettings(const String& fileName)
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
	void UISystemLayer::SaveUISettings(const String& fileName)
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

	EScheduleResult UISystemLayer::MenuUpdate()
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
		ImGui::Begin("UISystemLayer", nullptr, window_flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);
		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("UISystemLayer DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			UIManager::GetInstance().ForEach(UISystemLayer::BeginMenu, UISystemLayer::EndMenu);
			ImGui::EndMenuBar();
		}

		ImGui::End();
		return EScheduleResult::Continue;
	}

	void UISystemLayer::BeginMenu(const MenuItemNode* Node)
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

	void UISystemLayer::EndMenu(const MenuItemNode* Node)
	{
		if (Node->MenuItem == nullptr && Node->IsOpen)
		{
			ImGui::EndMenu();
		}
	}

}