#pragma once
#include "JGCore.h"



namespace JG
{
	class IUIViewModel;
	class UIViewModel;
	// Param / Child
#define UIVIEWCLASS 		virtual Type GetType() const override { return Type(TypeID(this));}
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
		virtual void Load()        = 0;
		virtual void Initialize()  = 0;
		virtual void OnGUI()       = 0;
		virtual void Destroy()     = 0;
	protected:
		virtual void OnEvent(IEvent& e) = 0;
	public:
		virtual void Open()  = 0;
		virtual void Close() = 0;
		virtual bool IsOpen() const = 0;
		virtual Type GetType() const = 0;
	public:
		virtual ~IUIView() = default;
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
		bool mIsLoad = false;
	private:
		virtual void OnEvent(IEvent& e) override {
			static_cast<IUIViewModel*>(mViewModel.get())->OnEvent(e);
		}
	protected:
		virtual void Load() override {};
		virtual void Initialize() override {};
		virtual void OnGUI() override {}
		virtual void Destroy() override {}
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

				if (mIsLoad == false)
				{
					Load();
					mIsLoad = true;
				}
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
	public:
		virtual ~UIView() = default;
	};
}