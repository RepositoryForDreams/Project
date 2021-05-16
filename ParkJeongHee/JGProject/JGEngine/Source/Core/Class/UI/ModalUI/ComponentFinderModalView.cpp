#include "pch.h"
#include "ComponentFinderModalView.h"
#include "Class/Game/GameObjectFactory.h"
namespace JG
{

	void ComponentFinderModalView::Initialize(const ComponentFinderInitData& data)
	{
		ImGui::OpenPopup(std::to_string(GetType().GetID()).c_str());
		mSelectedComponentType.clear();
	}
	void ComponentFinderModalView::Destroy()
	{
		mFilterStr.clear();
		mComponentTypeSet.clear();
		mFilteredTypeList.clear();
		mFilterStr.resize(256);
	}
	bool ComponentFinderModalView::OnGUI()
	{
		if (ImGui::BeginPopupContextWindow(std::to_string(GetType().GetID()).c_str()) == true)
		{
			char buf[256] = { 0, };
			ImGui::InputText("##Finder Serach", buf, 256);
			ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

			ImGui::BeginChild("##ComponentList", ImVec2(0, 600.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoMove);

			FindComponentTypeList(s2ws(buf));
			bool _bool = false;
			for (auto& type : mFilteredTypeList)
			{
				auto name = ReplaceAll(type, TT("JG::"), TT(""));
				if (ImGui::Selectable(ws2s(name).c_str(), &_bool))
				{
					mSelectedComponentType = type;
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndChild();
			ImGui::EndPopup();
			return true;
		}
		else
		{
			return false;
		}
	}
	const String& ComponentFinderModalView::GetSelectedComponent() const
	{
		return mSelectedComponentType;
	}
	void ComponentFinderModalView::FindComponentTypeList(const String& filter)
	{
		bool isDirty = false;
		auto& originSet = GameObjectFactory::GetInstance().GetGameComponentTypeSet();
		isDirty |= originSet.size() != mComponentTypeSet.size();
		isDirty |= mFilterStr != filter;

		if (isDirty)
		{
			mFilteredTypeList.clear();
			mComponentTypeSet = originSet;
			mFilterStr = filter;
			for (auto& type : originSet)
			{
				if (mFilterStr.length() == 0)
				{
					mFilteredTypeList.insert(type.GetName());
				}
				auto originName = ReplaceAll(type.GetName(), TT("JG::"), TT(""));
				auto filterName = filter;

				u64 pos = originName.find(filterName);
				if (pos != String::npos)
				{
					mFilteredTypeList.insert(type.GetName());
				}
			}
		}

	}

}