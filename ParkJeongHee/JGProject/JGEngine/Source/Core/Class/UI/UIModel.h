#pragma once



namespace JG
{
	class IUIModel
	{
		friend class UIViewModel;
	protected:
		virtual bool Initialize() = 0;
		virtual void Destroy() = 0;
	};
	// ���� ������ ����
	class UIModel : public IUIModel
	{

	};
}