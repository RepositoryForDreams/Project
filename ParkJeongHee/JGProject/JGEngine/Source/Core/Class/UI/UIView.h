#pragma once
#include "UIView.h"



namespace JG
{
	// Param / Child
	class IUIView
	{
		//
	public:
		virtual void OnGUI() = 0;
	};

	// 직접적으로 보여주는 것
	// UIView 는 UIModel의  존재를 몰라야한다.
	// UIViewModel 을 템플릿으로
	template<class ViewModelType>
	class UIView
	{
		// 자식
		// 
	};
}