#include "JGEditor.h"
#include "Layer/EditorLayer.h"


namespace JG
{
	void JGEditor::OpenImpl()
	{
		PushLayer(CreateSharedPtr<EditorLayer>());
	}

	void JGEditor::CloseImpl()
	{
	}
}
