#include "pch.h"
#include "ContentsView.h"


namespace JG
{
	ContentsView::ContentsView()
	{
		UIManager::GetInstance().RegisterMainMenuItem(TT("Windows/ContentsView"), 0, [&]()
		{
			Open();
		}, nullptr);



	}
	void ContentsView::Load()
	{
		// Copy Paste Delete Move �������� �ϴ� ����
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), TT("NewFolder"), 0, [&]() {
			if (mVm != nullptr)
			{
				mVm->NewFolder->Execute();
			}
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), TT("Copy"), 20, [&]() {}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), TT("Paste"), 20, [&]() {}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), TT("Move"), 20, [&]() {}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), TT("Delete"), 20, [&]() 
		{
			if (mVm != nullptr)
			{
				mVm->Delete->Execute();
			}
		}, nullptr);
	}
	void ContentsView::Initialize()
	{
		mIsColumnInit = true;
		mVm = GetViewModel();
	}
	void ContentsView::OnGUI()
	{
		ImGui::Begin("ContentsView", &mOpenGUI);
		// ��� ��ư

		ImGui::Button("Button01"); ImGui::SameLine(); ImGui::Button("Button02");
		ImGui::Separator();

		// �ΰ��� ������ ���ϵ� ������
		ImGui::Columns(2, nullptr, true);
		if (mIsColumnInit)
		{
			mIsColumnInit = false;
			f32 winWidth = ImGui::GetWindowWidth();
			ImGui::SetColumnWidth(0, winWidth * 0.25f);
		}
		ImGui::BeginChild("DirectoryPanel", ImVec2(0, 0), true);
		OnGui_ContentsDirectory();
		// ForEach �� �����ִ� ���丮�� ǥ��
		ImGui::EndChild();


		ImGui::NextColumn();
		ImGui::BeginChild("DetailPanel", ImVec2(0, 0), true);

		OnGui_ContentsDirectoryDetail();
		// ForEach�� �����ִ� ���ϵ鸸 ǥ��
		ImGui::EndChild();
		


		ImGui::End();

		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}
	void ContentsView::Destroy()
	{
	}
	void ContentsView::OnGui_ContentsDirectory()
	{
		auto Vm = GetViewModel();
		if (Vm == nullptr)
		{
			return;
		}

		Vm->ForEeach(
			[&](ContentsDirectoryNode* node) -> bool
		{
			bool isOpen = false;
			auto fileInfo = Vm->GetContentsFileInfo(node->Path);
			if (fileInfo == nullptr)
			{
				return false;
			}
			bool isRoot = fileInfo->OwnerDirectory == nullptr;
			bool isLeaf = fileInfo->DirectoryList.size() == 0;
			bool isSelectedInfo = Vm->IsSelectedContentsDirectory(fileInfo);

			node->UserFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

			(isLeaf) ?
				node->UserFlags |= ImGuiTreeNodeFlags_Leaf :
				node->UserFlags &= ~(ImGuiTreeNodeFlags_Leaf);
			(isSelectedInfo) ?
				node->UserFlags |= ImGuiTreeNodeFlags_Selected :
				node->UserFlags &= ~ImGuiTreeNodeFlags_Selected;

			if (isRoot == true)
			{
				isOpen = ImGui::CollapsingHeader(ws2s(fileInfo->Name + TT("##ContentsDirectory")).c_str());
				node->IsTreePop = false;
			}
			else
			{
				isOpen = ImGui::TreeNodeEx((void*)node, node->UserFlags, ws2s(fileInfo->Name).c_str());
				node->IsIgnoreSelect = (isOpen != node->IsTreePop);
				node->IsTreePop = isOpen;
			}

			return isOpen;
		},
			[&](ContentsDirectoryNode* node)
		{
			static bool isContextOpen = false;
			ImGui::PushID(node);
			node->IsSelected = ImGui::IsItemClicked();
			if (UIManager::GetInstance().ShowContextMenu(GetType()) == true)
			{
				node->IsTarget = true;
			}
			else
			{
				node->IsTarget = false;
			}


			ImGui::PopID();
		},
			[&](ContentsDirectoryNode* node)
		{
			ImGui::TreePop();
		});
	}
	void ContentsView::OnGui_ContentsDirectoryDetail()
	{
		auto Vm = GetViewModel();
		if (Vm == nullptr)
		{
			return;
		}
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.0f, 0.5f));
		Vm->ForEach([&](ContentsFileInfo* fileInfo)
		{
		
			ImGui::Dummy(ImVec2(20.0f, 20.0f)); ImGui::SameLine();
			
			if (ImGui::Selectable(ws2s(fileInfo->Name).c_str(), false, ImGuiSelectableFlags_None, ImVec2(0, 20.0f)) == true)
			{

			}

		});
		ImGui::PopStyleVar();
	}
}
