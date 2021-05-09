#pragma once
#include "JGCore.h"



namespace JG
{
	class IUIViewModel;
	class UIViewModel;


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


	class IUIView : public IJGObject, public IUIErrorReceiver
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
	public:
		virtual ~IUIView() = default;
	};

	template<class ViewModelType>
	class UIView : public IUIView
	{
		JGCLASS
	private:
		UniquePtr<ViewModelType> mViewModel;
		bool mIsOpen = false;
		bool mIsLoad = false;
	private:
		virtual void OnEvent(IEvent& e) override {
			static_cast<IUIViewModel*>(mViewModel.get())->OnEvent(e);
		}
	protected:
		virtual void Load()		  override {}
		virtual void Initialize() override {}
		virtual void OnGUI()	  override {}
		virtual void Destroy()	  override {}
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


		// IUIErrorReceiver
		virtual void ReceiveError(SharedPtr<IUIError> error) override {}

	public:
		virtual ~UIView() = default;
	};


	class IModalUIView : IJGObject
	{
		friend class UIManager;
	protected:
		virtual bool OnGUI()      = 0;
		virtual void OnEvent(IEvent& e) = 0;
	protected:
		virtual bool IsOpen() const = 0;
	public:
		virtual ~IModalUIView() = default;
	};

	template<class PopupInitData>
	class ModalUIView : public IModalUIView
	{
		friend class UIManager;
		bool mIsOpen = false;
	protected:
		virtual bool OnGUI() override { return false; }
		virtual void OnEvent(IEvent& e) override {}
	protected:
		virtual void Initialize(const PopupInitData& data) = 0;
		virtual void Destroy() = 0;
	private:
		virtual void Open(const PopupInitData& data)
		{
			if (mIsOpen == false)
			{
				mIsOpen = true;
				Initialize(data);
			}
		}
		virtual void Close()
		{
			if (mIsOpen == true)
			{
				mIsOpen = false;
				Destroy();
			}
		}
		virtual bool IsOpen() const
		{
			return mIsOpen;
		}
	public:
		virtual ~ModalUIView() = default;
	};
}