#pragma once
#include "JGCore.h"



namespace JG
{
	class IUIViewModel;
	class UIViewModel;
	// Param / Child

	class IUIError
	{
	public:
		virtual u64    GetID()    const = 0;
		virtual String ToString() const = 0;
	};

	class IUIErrorReceiver
	{
		friend UIViewModel;
	protected:
		virtual void ReceiveError(SharedPtr<IUIError> error) = 0;
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
	class UIView : public IUIView, public IUIErrorReceiver
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
				if (mViewModel == nullptr)
				{
					mViewModel = CreateUniquePtr<ViewModelType>();
				}
				static_cast<IUIViewModel*>(mViewModel.get())->Initialize();
				Initialize();
			}
		}
		virtual void Close() override
		{
			if (mIsOpen == true)
			{
				mIsOpen = false;
				static_cast<IUIViewModel*>(mViewModel.get())->Destroy();
				Destroy();
			}
		}
		virtual void ReceiveError(SharedPtr<IUIError> error) override {}
	};
}