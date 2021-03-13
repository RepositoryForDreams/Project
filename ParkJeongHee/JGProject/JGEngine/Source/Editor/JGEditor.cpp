#include "JGEditor.h"
#include "Layer/EditorLayer.h"
#include "Layer/DevLayer.h"

namespace JG
{
	void JGEditor::OpenImpl()
	{
		//PushLayer(CreateSharedPtr<EditorLayer>());
		PushLayer(CreateSharedPtr<DevLayer>());
	}

	void JGEditor::CloseImpl()
	{
	}
}
