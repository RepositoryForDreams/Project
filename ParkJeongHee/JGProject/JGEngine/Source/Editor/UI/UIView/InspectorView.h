#pragma once
#include "Class/UI/UIView.h"
#include "UI/UIViewModel/InspectorViewModel.h"


namespace JG
{
	
	class InspectorView : public UIView<InspectorViewModel>
	{
		UIVIEWCLASS

		bool mOpenGUI = true;
		bool mIsOpenPopup = false;
		InspectorViewModel* mVm = nullptr;


		//Dictionary<String, bool> m
		char mFindFilterStr[256];
	public:
		InspectorView();
		virtual ~InspectorView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
	private:

	};
}


