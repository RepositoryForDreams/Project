#include "pch.h"
#include "InspectorView.h"
#include "Imgui/imgui.h"
#include "Class/Game/GameObject.h"
#include "UI/UIModel/InspectorModel.h"


namespace JG
{
	InspectorView::InspectorView()
	{
		UIManager::GetInstance().RegisterMainMenuItem(TT("Windows/Inspector"), 0, [&]()
		{
			Open();
		}, nullptr);
	}
	void InspectorView::Load()
	{

	}
	void InspectorView::Initialize()
	{
		mVm = GetViewModel();
	}
	void InspectorView::OnGUI()
	{
		ImGui::Begin("Inspector", &mOpenGUI);
		if (mVm != nullptr)
		{
			GameObject* gameObject = mVm->GetTargetGameObject();
			if (gameObject != nullptr)
			{
				ImGui::Dummy(ImVec2(0, 1.0f));
				char objName[128] = { 0, };
				strcpy(objName, ws2s(gameObject->GetName()).c_str());
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Name "); ImGui::SameLine(); 
				if (ImGui::InputText("##Inspector_Name_InputText", objName, 128))
				{
					gameObject->SetName(s2ws(objName));
				} 

				ImGui::Text("ObjectID  :  %lu", gameObject->GetID());

				ImGui::Dummy(ImVec2(0, 1.0f));
				ImGui::Separator();
				
				UIManager::GetInstance().OnInspectorGUI(
					gameObject->GetType(),
					gameObject->GetInspectorDataType(), 
					gameObject->GetInspectorData());
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