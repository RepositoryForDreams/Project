#include "JGEditor.h"
#include "Layer/EditorLayer.h"
#include "Layer/DevLayer.h"

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
		//PushLayer(CreateSharedPtr<EditorLayer>());
		PushLayer(CreateSharedPtr<DevLayer>());
	}
}
