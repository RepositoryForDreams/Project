#pragma once
#include "JGCore.h"




namespace JG
{
	// 
	class IUIViewModel
	{


	protected:
		virtual bool Initialize() = 0;
		virtual void Destroy()    = 0;
	};

	// UI���� �˰��ִ�. UI�𵨰� ��ȣ�ۿ��ϸ鼭 ����������  View�� �ʿ��� ������ �������ش�.
	class UIViewModel
	{


	};
}