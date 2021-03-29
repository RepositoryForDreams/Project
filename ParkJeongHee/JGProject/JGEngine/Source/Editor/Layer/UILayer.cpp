#include "pch.h"
#include "UILayer.h"
#include "UI/UIView/StatisticsView.h"

namespace JG
{
	void UILayer::OnAttach()
	{
	
	}
	void UILayer::OnDetach()
	{

	}


	void UILayer::Begin()
	{
		UIManager::GetInstance().RegisterUIView<StatisticsView>();
	}
	void UILayer::Destroy()
	{

	}
	void UILayer::OnEvent(IEvent& e)
	{

	}
	String UILayer::GetLayerName()
	{
		return TT("UILayer");
	}
}