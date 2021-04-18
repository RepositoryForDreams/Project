#include "pch.h"
#include "InspectorView.h"
#include "Imgui/imgui.h"
#include "Class/Game/GameObjectFactory.h"
#include "Class/Game/GameNode.h"
#include "UI/UIModel/InspectorModel.h"


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
			GameNode* gameNode = mVm->GetTargetGameNode();
			if (gameNode != nullptr)
			{
				ImGui::Dummy(ImVec2(0, 1.0f));
				char objName[128] = { 0, };
				strcpy(objName, ws2s(gameNode->GetName()).c_str());
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Name "); ImGui::SameLine(); 
				if (ImGui::InputText("##Inspector_Name_InputText", objName, 128))
				{
					gameNode->SetName(s2ws(objName));
				} 

				ImGui::Text("ObjectID  :  %lu", gameNode->GetID());

				ImGui::Dummy(ImVec2(0, 1.0f));
				ImGui::Separator();

				gameNode->OnInspectorGUI();

				ImGui::Spacing();	ImGui::Spacing();	ImGui::Spacing();
				auto padding = ImGui::GetStyle().FramePadding;
				if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowSize().x - (padding.x * 4), 0)) == true)
				{
					memset(mFindFilterStr, 0, 256);
					ImGui::OpenPopup("Component Finder");
					mIsOpenPopup = true;
				}
				if (mIsOpenPopup == true)
				{
					if (ImGui::BeginPopupContextWindow("Component Finder") == true)
					{
						ImGui::InputText("##Finder Serach", mFindFilterStr, 256);
						ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

						ImGui::BeginChild("##ComponentList", ImVec2(0, 600.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoMove);

						auto comTypeList = mVm->FindComponentTypeList(s2ws(mFindFilterStr));
						bool _bool = false;
						for (auto& type : comTypeList)
						{
							if (ImGui::Selectable(ws2s(type).c_str(), &_bool))
							{
								mVm->SelectComponentType(type);
								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndChild();


						ImGui::EndPopup();
					}
					else
					{
						mIsOpenPopup = false;
					}
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