#include "pch.h"
#include "MainMenuLayer.h"
#include "Imgui/imgui.h"

namespace JG
{
	void MainMenuLayer::OnAttach()
	{

	}
	void MainMenuLayer::OnDetach()
	{

	}
	void MainMenuLayer::Update()
	{
		if (ImGui::BeginMainMenuBar())
		{
			// Menu 
			if (ImGui::BeginMenu("Debug"))
			{



			}




			ImGui::EndMainMenuBar();
		}


	}
	void MainMenuLayer::LateUpdate()
	{
	}
	void MainMenuLayer::Begin()
	{

	}
	void MainMenuLayer::Destroy()
	{
	}
	void MainMenuLayer::OnEvent(IEvent& e)
	{
	}
	String MainMenuLayer::GetLayerName()
	{
		return TT("MainMenuLayer");
	}
}