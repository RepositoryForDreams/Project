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

		auto json = CreateSharedPtr<Json>();
		Json::Read(fileName, json);

		UIManager::GetInstance().ForEach([&](IUIView* view)
		{
			auto key = view->GetType().GetName();
			auto member = json->GetMember(key);
			if (member != nullptr)
			{
				bool isOpen = member->GetBool();
				if (isOpen == true) view->Open();
				else view->Close();
			}
		});

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

		auto json = CreateSharedPtr<Json>();

		
		UIManager::GetInstance().ForEach([&](IUIView* view)
		{
			json->AddMember<bool>(view->GetType().GetName(), view->IsOpen());
		});

		Json::Write(fileName, json);
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
		// ShortCut
		if (Node->MenuItem != nullptr && Node->MenuItem->ShortCut.empty() == false)
		{
			auto shortCut = Node->MenuItem->ShortCut;
			bool isCtrl  = shortCut.find(TT("Ctrl")) != String::npos;
			bool isShift = shortCut.find(TT("Shift")) != String::npos;
			bool isAlt   = shortCut.find(TT("Alt")) != String::npos;
			shortCut = ReplaceAll(shortCut, TT("Ctrl"), TT(""));
			shortCut = ReplaceAll(shortCut, TT("Shift"), TT(""));
			shortCut = ReplaceAll(shortCut, TT("Alt"), TT(""));
			shortCut = ReplaceAll(shortCut, TT(" "), TT(""));
			shortCut = ReplaceAll(shortCut, TT("+"), TT(""));

			bool isPressed = false;
			bool isSubPressed = false;
			
			if (isCtrl)  isSubPressed |= ImGui::IsKeyDown((int)EKeyCode::Ctrl);
			if (isShift) isSubPressed |= ImGui::IsKeyDown((int)EKeyCode::Shift);
			if (isAlt)   isSubPressed |= ImGui::IsKeyDown((int)EKeyCode::Alt);
			
			isPressed = ImGui::IsKeyPressed((int)StringToKeyCode(shortCut));
			if ((isPressed && isSubPressed )== true)
			{
				Node->MenuItem->Action();
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