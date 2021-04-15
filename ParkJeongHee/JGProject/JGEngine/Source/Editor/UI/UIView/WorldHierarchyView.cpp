#include "pch.h"
#include "WorldHierarchyView.h"
#include "Class/Game/GameNode.h"
#include "Imgui/imgui.h"
#include "Layer/UILayer.h"
namespace JG
{
	WorldHierarchyView::WorldHierarchyView()
	{
		UIManager::GetInstance().RegisterMainMenuItem(TT("Windows/WorldHierarchyView"), 0, [&]()
		{
			Open();
		}, nullptr);


	}
	void WorldHierarchyView::Load()
	{
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), TT("2D"), 0, nullptr, nullptr);
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
		if (ImGui::Begin("WorldHierarchyView", &mOpenGUI))
		{
			mVm->ForEach(
				[&](WorldHierarchyTreeNode& nodeData) -> bool
			{
				bool isOpen = false;
				bool isRoot = nodeData.Object->GetParent() == nullptr;
				bool isLeaf = nodeData.Object->GetChildCount() == 0;
				nodeData.UserFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
				(isRoot) ?
					nodeData.UserFlags |= (ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader) :
					nodeData.UserFlags &= ~(ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader);

				(isLeaf) ?
					nodeData.UserFlags |= ImGuiTreeNodeFlags_Leaf :
					nodeData.UserFlags &= ~(ImGuiTreeNodeFlags_Leaf);
				
				isOpen = ImGui::TreeNodeEx(ws2s(TT("World : ") + nodeData.Object->GetName()).c_str(), nodeData.UserFlags);

				if (isRoot && isLeaf)
				{
					return false;
				}

				return isOpen;
			},
				[&](WorldHierarchyTreeNode& nodeData)
			{
				if (ImGui::IsItemClicked() == true)
				{
					nodeData.IsSelected = true;
				}
				else if (ImGui::IsKeyDown((i32)EKeyCode::Ctrl) == false)
				{
					nodeData.IsSelected = false;
				}
				(nodeData.IsSelected == true) ?
					nodeData.UserFlags |= ImGuiTreeNodeFlags_Selected :
					nodeData.UserFlags &= ~ImGuiTreeNodeFlags_Selected;
	
				UILayer::ShowContextMenu(GetType());
			},
				[&](WorldHierarchyTreeNode& nodeData)
			{
				ImGui::TreePop();
			});

			ImGui::End();
		}


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

