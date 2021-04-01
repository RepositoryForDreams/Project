#include "pch.h"
#include "GraphicsLayer.h"

namespace JG
{
	void GraphicsLayer::OnAttach()
	{
	}

	void GraphicsLayer::OnDetach()
	{
	}

	void GraphicsLayer::Begin()
	{
	}

	void GraphicsLayer::Destroy()
	{
	}

	void GraphicsLayer::OnEvent(IEvent& e)
	{
	}

	String GraphicsLayer::GetLayerName()
	{
		return String();
	}

	EScheduleResult GraphicsLayer::Update()
	{
		return EScheduleResult();
	}
}

