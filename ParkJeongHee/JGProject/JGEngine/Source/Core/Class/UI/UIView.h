#pragma once
#include "JGCore.h"



namespace JG
{
	class UIViewModel;
	// Param / Child

	template<class ErrorDataType>
	class IUIErrorReceiver
	{
	protected:
		virtual void ReceiveError(const ErrorDataType& error) = 0;
	};


	class IUIView
	{
		friend class UIManager;
	protected:
		virtual bool Initialize()  = 0;
		virtual void OnGUI()       = 0;
		virtual void Destroy()     = 0;
		virtual Type GetType() const = 0;

		virtual void Open()  = 0;
		virtual void Close() = 0;
		virtual bool IsOpen() const = 0;
	};

	
	// 직접적으로 보여주는 것
	// UIView 는 UIModel의  존재를 몰라야한다.
	// UIViewModel 을 템플릿으로
	template<class ViewModelType>
	class UIView : public IUIView
	{
	private:
		UniquePtr<ViewModelType> mViewModel;
		bool mIsOpen = false;
	public:
		ViewModelType* GetViewModel() const {
			return mViewModel.get();
		}
		virtual bool IsOpen() const override {
			return mIsOpen;
		}
		virtual void Open() override
		{
			if (mIsOpen == false)
			{
				mIsOpen = true;
				Initialize();
			}
		}
		virtual void Close() override
		{
			if (mIsOpen == true)
			{
				mIsOpen = false;
				Destroy();
			}
		}
	};
}