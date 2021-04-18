#include "pch.h"
#include "WorldHierarchyView.h"
#include "Class/Game/GameNode.h"
#include "Imgui/imgui.h"
#include "Layer/UILayer.h"
namespace JG
{
	WorldHierarchyView::WorldHierarchyView()
	{
		UIManager::GetInstance().RegisterMainMenuItem(TT("Windows/WorldHierarchy"), 0, [&]()
		{
			Open();
		}, nullptr);
	}
	void WorldHierarchyView::Load()
	{


		UIManager::GetInstance().RegisterContextMenuItem(GetType(), TT("Empty Object"), 0, [&]()
		{
			mVm->GetCommand_AddEmptyObject()->Execute(mVm->GetSelectdNodeInContextMenu());
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), TT("2D/Sprite"), 0, nullptr, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), TT("3D/Cube"), 0, nullptr, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), TT("3D/Sphere"), 0, nullptr, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), TT("3D/UnityChan"), 0, nullptr, nullptr);
	}
	void WorldHierarchyView::Initialize()
	{
		mVm = GetViewModel();
	}
	void WorldHierarchyView::OnGUI()
	{
		ImGui::Begin("WorldHierarchy", &mOpenGUI);

		mVm->ForEach(
			[&](WorldHierarchyTreeNode& nodeData) -> bool
		{
			bool isOpen = false;
			bool isRoot = nodeData.Object->GetParent() == nullptr;
			bool isLeaf = nodeData.Object->GetChildCount() == 0;
			nodeData.UserFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

			(isLeaf) ?
				nodeData.UserFlags |= ImGuiTreeNodeFlags_Leaf :
				nodeData.UserFlags &= ~(ImGuiTreeNodeFlags_Leaf);

			if (isRoot == true)
			{
				isOpen = ImGui::CollapsingHeader(ws2s(nodeData.Object->GetName() + TT("##GameWorld")).c_str());
				nodeData.IsTreePop = false;
			}
			else
			{
				isOpen = ImGui::TreeNodeEx((void*)nodeData.Object->GetID(), nodeData.UserFlags, ws2s(nodeData.Object->GetName()).c_str());
				nodeData.IsTreePop = isOpen;
			}

			return isOpen;
		},
			[&](WorldHierarchyTreeNode& nodeData)
		{
			static bool isContextOpen = false;
			ImGui::PushID(nodeData.Object->GetID());
			if (UILayer::ShowContextMenu(GetType()) == true)
			{
				if (isContextOpen == false)
				{
					isContextOpen = true;
					mVm->SetSelectedNodeInContextMenu(nodeData.Object);
				}

			}
			else
			{
				isContextOpen = false;
				if (ImGui::IsItemClicked() == true)
				{
					mVm->SetSelectedNodeInInspector(nodeData.Object);
				}
			}
			(nodeData.IsSelected == true) ?
				nodeData.UserFlags |= ImGuiTreeNodeFlags_Selected :
				nodeData.UserFlags &= ~ImGuiTreeNodeFlags_Selected;

			ImGui::PopID();
		},
			[&](WorldHierarchyTreeNode& nodeData)
		{
			ImGui::TreePop();
		});
		ImGui::End();


		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}

	void WorldHierarchyView::Destroy()
	{

	}
}

