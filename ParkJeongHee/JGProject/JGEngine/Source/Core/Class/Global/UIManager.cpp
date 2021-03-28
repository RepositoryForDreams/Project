#include "pch.h"
#include "UIManager.h"
#include "Class/UI/UIView.h"

namespace JG
{
	UIManager::UIManager()
	{

	}
	UIManager::~UIManager()
	{
		for (auto& _pair : mUIViewPool)
		{
			if (_pair.second->IsOpen())
			{
				_pair.second->Close();
			}
		}
		mUIViewPool.clear();
	}
	void UIManager::Update()
	{


	}
	void UIManager::ForEach(const std::function<void(IUIView*)> action)
	{
		for (auto& _pair : mUIViewPool)
		{
			action(_pair.second.get());
		}
	}
}

