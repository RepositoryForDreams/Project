#pragma once
#include "JGCore.h"
namespace JG
{
	class IUIView;
	class UIManager : public GlobalSingleton<UIManager>
	{
	private:
		Dictionary<Type, SharedPtr<IUIView>> mUIViewPool;
		std::shared_mutex mMutex;
	public:
		UIManager();
		~UIManager();
	private:
		friend class Application;
		void Update();
	public:
		// µî·Ï
		template<class UIViewType>
		void RegisterUIView()
		{
			Type type = Type(TypeID<UIViewType>());


			std::lock_guard<std::shared_mutex> lock(mMutex);
			if (mUIViewPool.find(type) != mUIViewPool.end())
			{
				return;
			}
			mUIViewPool[type] = CreateSharedPtr<UIViewType>();
		}


		template<class UIViewType>
		WeakPtr<UIViewType> GetUIView() const
		{
			Type type = Type(TypeID<UIViewType>());

			std::shared_lock<std::shared_mutex> lock(mMutex);
			auto iter = mUIViewPool.find(type);
			if (iter == mUIViewPool.end())
			{
				JG_CORE_ERROR("Not Find UIViewType : {0}", s2ws(type.GetName()));
				return nullptr;
			}
			return iter->second;
		}

		void ForEach(const std::function<void(IUIView*)> action);
	};
}