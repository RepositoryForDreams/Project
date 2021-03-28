#pragma once



namespace JG
{
	class IUIModel
	{
	protected:
		virtual bool Initialize() = 0;
		virtual void Destroy() = 0;
	};
	// 보통 데이터 정의
	class UIModel
	{

	};
}