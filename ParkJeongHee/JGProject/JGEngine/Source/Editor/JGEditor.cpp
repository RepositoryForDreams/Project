#include "pch.h"
#include "JGEditor.h"
#include "Layer/DevLayer.h"
#include "Layer/ImGuiLayer.h"
#include "Layer/MainMenuLayer.h"
#include "Layer/UILayer.h"
#include "Layer/GraphicsLayer.h"
#include "Layer/GameLayer.h"
namespace JG
{
	void JGEditor::OpenImpl()
	{

	}

	void JGEditor::CloseImpl()
	{
	}
	void JGEditor::LoadImpl()
	{
		PushLayer(CreateSharedPtr<ImGuiLayer>());
		PushLayer(CreateSharedPtr<MainMenuLayer>());
		PushLayer(CreateSharedPtr<UILayer>());
		PushLayer(CreateSharedPtr<GameLayer>());
		PushLayer(CreateSharedPtr<GraphicsLayer>());

		//PushLayer(CreateSharedPtr<DevLayer>());
	}
}
