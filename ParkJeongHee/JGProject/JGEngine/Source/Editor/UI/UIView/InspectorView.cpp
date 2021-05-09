#include "pch.h"
#include "InspectorView.h"
#include "Imgui/imgui.h"
#include "Class/Game/GameObjectFactory.h"
#include "Class/Game/GameNode.h"
#include "UI/UIModel/InspectorModel.h"
#include "UI/ModalUI/ComponentFinderModalView.h"

namespace JG
{
	InspectorView::InspectorView()
	{
		UIManager::GetInstance().RegisterMainMenuItem(TT("Windows/Inspector"), 0, [&]()
		{
			Open();
		}, nullptr);
		memset(mFindFilterStr, 0, 256);
	}
	void InspectorView::Load()
	{

	}
	void InspectorView::Initialize()
	{
		mVm = GetViewModel();
		memset(mFindFilterStr, 0, 256);
	}
	void InspectorView::OnGUI()
	{
		ImGui::Begin("Inspector", &mOpenGUI);
		if (mVm != nullptr)
		{
			IJGObject* object = mVm->GetTargetObject();
			if (object != nullptr)
			{


				bool isGameNode = GameObjectFactory::GetInstance().IsGameNode(object->GetType());
				object->OnInspectorGUI();

				ImGui::Spacing();	ImGui::Spacing();	ImGui::Spacing();

				if (isGameNode == true)
				{
					auto gameNode = static_cast<GameNode*>(object);
					if (gameNode->GetParent() != nullptr)
					{
						auto padding = ImGui::GetStyle().FramePadding;
						if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowSize().x - (padding.x * 4), 0)) == true)
						{
							UIManager::GetInstance().OpenModalUIView<ComponentFinderModalView>(ComponentFinderInitData());
						}
					}
				}
				if (UIManager::GetInstance().OnModalUIView<ComponentFinderModalView>())
				{
					auto comFinder = UIManager::GetInstance().GetModalUIView<ComponentFinderModalView>();
					auto selectedType = comFinder->GetSelectedComponent();
					mVm->SelectComponentType(selectedType);
				}
			}
			else
			{
				
			}
		}

		ImGui::End();
		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}
	void InspectorView::Destroy()
	{
	}
}