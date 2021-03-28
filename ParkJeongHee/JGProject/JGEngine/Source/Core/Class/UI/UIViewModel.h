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

	// UI모델을 알고있다. UI모델과 상호작용하면서 지속적으로  View에 필요한 정보를 가공해준다.
	class UIViewModel
	{


	};
}