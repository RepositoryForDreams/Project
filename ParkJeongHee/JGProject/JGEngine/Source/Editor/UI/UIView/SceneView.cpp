#include "pch.h"
#include "SceneView.h"
#include "ExternalImpl/JGImGui.h"
#include "Imgui/imgui.h"

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
	}

	void SceneView::OnGUI()
	{
		auto viewModel = GetViewModel();
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