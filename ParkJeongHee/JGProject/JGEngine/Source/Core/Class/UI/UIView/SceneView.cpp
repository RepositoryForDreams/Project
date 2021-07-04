#include "pch.h"
#include "SceneView.h"
#include "Class/Game/GameNode.h"
#include "Class/Game/Components/Transform.h"
#include "Class/Game/Components/Camera.h"
#include "ExternalImpl/JGImGui.h"
#include "Imgui/imgui.h"
#include "Imgui/ImGuizmo.h"
namespace JG
{
	SceneView::SceneView()
	{
		UIManager::GetInstance().RegisterMainMenuItem(TT("Windows/SceneView"), 0,
			[&]()
		{
			Open();
		}, nullptr);
	}
	void SceneView::Load()
	{

	}
	void SceneView::Initialize()
	{
		auto viewModel = GetViewModel();
		viewModel->SetMinSize(JVector2(820, 620));


		viewModel->ShowGizmo->Subscribe(viewModel, [&](GameNode* node)
		{
		
			static ImGuizmo::OPERATION CurrentGizmoOperation(ImGuizmo::TRANSLATE);
			static ImGuizmo::MODE CurrentGizmoMode(ImGuizmo::LOCAL);


			auto mainCam = Camera::GetMainCamera();

			if (ImGui::IsKeyPressed((int)EKeyCode::Q))
				CurrentGizmoOperation = ImGuizmo::TRANSLATE;
			if (ImGui::IsKeyPressed((int)EKeyCode::W))
				CurrentGizmoOperation = ImGuizmo::ROTATE;
			if (ImGui::IsKeyPressed((int)EKeyCode::E)) 
				CurrentGizmoOperation = ImGuizmo::SCALE;


			if (mainCam == nullptr) return;
			if (node == nullptr || node->GetType() != JGTYPE(GameNode)) return;
			auto worldMat = node->GetTransform()->GetWorldMatrix();
			auto itemMin = ImGui::GetItemRectMin();
			auto itemSize = ImGui::GetItemRectSize();
			ImGuizmo::SetRect(itemMin.x, itemMin.y, itemSize.x, itemSize.y);

			auto view = mainCam->GetViewMatrix();
			auto proj = mainCam->GetProjMatrix();
			ImGuizmo::Manipulate(view.GetFloatPtr(), proj.GetFloatPtr(), CurrentGizmoOperation, CurrentGizmoMode, worldMat.GetFloatPtr(), NULL, NULL);
			JVector3 matrixTranslation, matrixRotation, matrixScale;
			ImGuizmo::DecomposeMatrixToComponents(worldMat.GetFloatPtr(), (float*)&matrixTranslation, (float*)&matrixRotation, (float*)&matrixScale);
			node->GetTransform()->SetLocalLocation(matrixTranslation);
			node->GetTransform()->SetLocalRotation(matrixRotation);
			node->GetTransform()->SetScale(matrixScale);
		});

	}
	void SceneView::OnGUI()
	{
		auto viewModel = GetViewModel();
		ImGuizmo::BeginFrame();
	
		ImGui::Begin("SceneView", &mOpenGUI);

		auto minSize = viewModel->GetMinSize();
		auto currSize = ImGui::GetWindowSize();

		if (currSize.x <= minSize.x || currSize.y <= minSize.y)
		{
			ImGui::SetWindowSize(ImVec2(minSize.x, minSize.y));
		}
		auto sceneTexture = viewModel->GetSceneTexture();

		if (sceneTexture != nullptr && sceneTexture->IsValid())
		{
			auto textureInfo = sceneTexture->GetTextureInfo();
			auto textureID = (ImTextureID)JGImGui::GetInstance().ConvertImGuiTextureID(sceneTexture->GetTextureID());
			ImGui::Image(textureID, ImVec2(textureInfo.Width, textureInfo.Height));
			viewModel->ShowGizmo->Execute(viewModel->GetSelectedGameNode());
		}

		if (ImGui::IsMouseClicked(0) == true) 
		{
			auto winPos   = ImGui::GetItemRectMin();
			auto mousePos = ImGui::GetMousePos();

			auto standardPos = JVector2(mousePos.x, mousePos.y) - JVector2(winPos.x ,winPos.y);
			viewModel->OnClick(standardPos, 0);
			JG_CORE_INFO("Scene View MousePos : {0}, {1}", standardPos.x, standardPos.y);
		}



		ImGui::End();
		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}

	void SceneView::Destroy()
	{
	}
}