#include "pch.h"
#include "WorldHierarchyView.h"
#include "Class/Game/GameNode.h"
#include "Imgui/imgui.h"

namespace JG
{
	WorldHierarchyView::WorldHierarchyView()
	{
		UIManager::GetInstance().RegisterMenuItem(TT("Windows/WorldHierarchyView"), 0, [&]()
		{
			Open();
		}, nullptr);
	}
	void WorldHierarchyView::Initialize()
	{
		mVm = GetViewModel();
	}

	void WorldHierarchyView::OnGUI()
	{
		if (ImGui::Begin("WorldHierarchyView", &mOpenGUI))
		{
			
			mVm->ForEach([&](GameNode* node)
			{
				if (ImGui::TreeNodeEx("sss"))
				{

					ImGui::TreePop();
				}
		
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

