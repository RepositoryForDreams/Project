#include "JGEditor.h"
#include "Layer/DevLayer.h"
#include "Layer/ImGuiLayer.h"
#include "Layer/MainMenuLayer.h"
#include "Layer/UILayer.h"

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
		PushLayer(CreateSharedPtr<DevLayer>());
	}
}
