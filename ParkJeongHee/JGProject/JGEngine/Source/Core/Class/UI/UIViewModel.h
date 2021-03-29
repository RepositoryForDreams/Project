#pragma once
#include "JGCore.h"




namespace JG
{
	// 
	class IUIModel;
	class IUIError;
	class IUIErrorReceiver;
	class IUIViewModel
	{
		template<class T>
		friend class UIView;
	protected:
		virtual bool Initialize() = 0;
		virtual void Destroy()    = 0;
		virtual void SetErrorReceiver(IUIErrorReceiver* receiver) = 0;
	};

	// UI���� �˰��ִ�. UI�𵨰� ��ȣ�ۿ��ϸ鼭 ����������  View�� �ʿ��� ������ �������ش�.


	class UIViewModel : public IUIViewModel
	{
	private:
		Dictionary<Type, SharedPtr<IUIModel>> mUIModelPool;
		IUIErrorReceiver* mUIErrorReceiver = nullptr;
	protected:
		template<class UIModelType>
		UIModelType* RegisterUIModel()
		{
			Type type = Type(TypeID<UIModelType>());
			if (mUIModelPool.find(type) != mUIModelPool.end())
			{
				return nullptr;
			}
			mUIModelPool[type] = CreateSharedPtr<UIModelType>();
			mUIModelPool[type]->Initialize();

			return static_cast<UIModelType*>(mUIModelPool[type].get());
		}

		template<class UIModelType>
		UIModelType* GetUIModel() const
		{
			Type type = Type(TypeID<UIModelType>());
			auto iter = mUIModelPool.find(type);
			if (iter == mUIModelPool.end())
			{
				return nullptr;
			}
			return static_cast<UIModelType*>(iter->second.get());
		}

		virtual void Destroy() override;
	protected:
		void SendError(SharedPtr<IUIError> error);
	private:
		virtual void SetErrorReceiver(IUIErrorReceiver* receiver) override;
		
	};
}