#include "pch.h"
#include "UILayer.h"
#include "UI/UIView/StatisticsView.h"
#include "UI/UIView/SceneView.h"
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
		UIManager::GetInstance().RegisterUIView<SceneView>();
		UIManager::GetInstance().RegisterUIView<StatisticsView>();
		LoadUISettings(TT("JGUI.ini"));
	}
	void UILayer::Destroy()
	{
		SaveUISettings(TT("JGUI.ini"));
	}
	void UILayer::OnEvent(IEvent& e)
	{

	}
	String UILayer::GetLayerName()
	{
		return TT("UILayer");
	}


	void UILayer::LoadUISettings(const String& fileName)
	{
		Dictionary<String, bool> IsOpen;

		FileStreamReader fileReader;
		if (fileReader.Open(fileName) == true)
		{
			fileReader.Read(&IsOpen);
			fileReader.Close();
		}
		if (IsOpen.empty() == false)
		{
			UIManager::GetInstance().ForEach([&](IUIView* view)
			{

				auto iter = IsOpen.find(view->GetType().GetName());
				if (iter != IsOpen.end())
				{
					if (iter->second)
					{
						view->Open();
					}
					else
					{
						view->Close();
					}
				}
			});
		}

	}
	void UILayer::SaveUISettings(const String& fileName)
	{

		Dictionary<String, bool> IsOpen;
		FileStreamWriter fileWriter;
		if (fileWriter.Open(fileName) == true)
		{
			UIManager::GetInstance().ForEach([&](IUIView* view)
			{
				IsOpen.emplace(view->GetType().GetName(), view->IsOpen());
			});

			fileWriter.Write(IsOpen);
			fileWriter.Close();
		}
		
	}

}