#pragma once



namespace JG
{
	class IUIModel
	{
		friend class UIViewModel;
	protected:
		virtual bool Initialize() = 0;
		virtual void Destroy() = 0;
	public:
		virtual ~IUIModel() = default;
	};
	// 보통 데이터 정의
	class UIModel : public IUIModel
	{
	public:
		virtual ~UIModel() = default;
	};
}