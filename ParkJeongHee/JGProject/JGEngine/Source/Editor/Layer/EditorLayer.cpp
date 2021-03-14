#include "EditorLayer.h"
#include "Application.h"

namespace JG
{
	void EditorLayer::OnAttach()
	{

	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::Update()
	{

	}

	void EditorLayer::LateUpdate()
	{
	}

	void EditorLayer::Begin()
	{

	}

	void EditorLayer::Destroy()
	{

	}

	void EditorLayer::OnEvent(IEvent& e)
	{
        EventDispatcher eventDispatcher(e);

        eventDispatcher.Dispatch<AppResizeEvent>(EVENT_BIND_FN(&EditorLayer::Resize));
	}

	String EditorLayer::GetLayerName()
	{
		return TT("JGEditor");
	}

	bool EditorLayer::Resize(AppResizeEvent& e)
	{
        return false;
	}
}
