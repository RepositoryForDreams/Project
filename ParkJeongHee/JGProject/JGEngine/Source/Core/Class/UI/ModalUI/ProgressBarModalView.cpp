#include "pch.h"
#include "ProgressBarModalView.h"
#include "Application.h"
namespace JG
{
	void ProgressBarModalView::Initialize(const ProgressBarInitData& data)
	{
		mIsOpenPopup = false;
		mTitle = data.Title;
		auto id = ws2s(mTitle + TT("##") + std::to_wstring(GetType().GetID()));
		
	}
	void ProgressBarModalView::Destroy()
	{

	}
	bool ProgressBarModalView::OnGUI()
	{
		auto id = ws2s(mTitle + TT("##") + std::to_wstring(GetType().GetID()));
		auto displaySize = ImGui::GetIO().DisplaySize;
		auto appPos = Application::GetInstance().GetWindow()->GetPosition();
		auto winSize = ImVec2(500, 55);


		if (mIsOpenPopup == false)
		{
			mIsOpenPopup = true;
			ImGui::OpenPopup(id.c_str());
		}
		
		ImGui::SetNextWindowSize(winSize);
		ImGui::SetNextWindowPos(ImVec2(appPos.x + displaySize.x * 0.5f - winSize.x * 0.5f, appPos.y + displaySize.y * 0.5f - winSize.y * 0.5f));
		if (ImGui::BeginPopupModal(id.c_str(), nullptr, ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			f32 padding = ImGui::GetStyle().FramePadding.x;

			ImGui::ProgressBar(mCurrProgressRatio, ImVec2(winSize.x - padding * 4 ,0), ws2s(mContents).c_str());

			ImGui::EndPopup();
			return true;
		}
		else
		{
			return false;
		}
	}
	void ProgressBarModalView::Display(const String& contents, f32 ratio)
	{
		mContents = contents;
		mCurrProgressRatio = ratio;
	}
}