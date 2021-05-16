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
	}
	void ContentsView::Initialize()
	{
		mIsColumnInit = true;
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
			f32 winWidth = ImGui::GetWindowWidth();
			ImGui::SetColumnWidth(0, winWidth * 0.3f);
		}
		ImGui::BeginChild("DirectoryPanel", ImVec2(0, 0), true);
		OnGui_ContentsDirectory();
		// ForEach �� �����ִ� ���丮�� ǥ��
		ImGui::EndChild();


		ImGui::NextColumn();
		ImGui::BeginChild("DetailPanel", ImVec2(0, 0), true);
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

			node->UserFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

			(isLeaf) ?
				node->UserFlags |= ImGuiTreeNodeFlags_Leaf :
				node->UserFlags &= ~(ImGuiTreeNodeFlags_Leaf);

			if (isRoot == true)
			{
				isOpen = ImGui::CollapsingHeader(ws2s(fileInfo->Name + TT("##ContentsDirectory")).c_str());
				node->IsTreePop = false;
			}
			else
			{
				isOpen = ImGui::TreeNodeEx((void*)node, node->UserFlags, ws2s(fileInfo->Name).c_str());
				node->IsTreePop = isOpen;
			}

			return isOpen;
		},
			[&](ContentsDirectoryNode* info)
		{
			ImGui::PushID(info);
			info->IsSelected = ImGui::IsItemClicked();
			ImGui::PopID();
		},
			[&](ContentsDirectoryNode* info)
		{
			ImGui::TreePop();
		});
	}
}

