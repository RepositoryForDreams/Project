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

	// ���������� �����ִ� ��
	// UIView �� UIModel��  ���縦 ������Ѵ�.
	// UIViewModel �� ���ø�����
	template<class ViewModelType>
	class UIView
	{
		// �ڽ�
		// 
	};
}