#include "pch.h"
#include "JGEditor.h"
#include "Layer/DevLayer.h"
#include "Layer/ImGuiLayer.h"
#include "Layer/UILayer.h"
#include "Layer/GraphicsLayer.h"
#include "Layer/GameLogicLayer.h"
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
		PushLayer(CreateSharedPtr<UILayer>());
		PushLayer(CreateSharedPtr<GameLogicLayer>());
		PushLayer(CreateSharedPtr<GraphicsLayer>());
	}
}
