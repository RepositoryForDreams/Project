#pragma once
#include "JGCore.h"
#include <Common/Type.h>
#include <shared_mutex>

namespace JG
{
	class IUIView;
	class UIManager : public GlobalSingleton<UIManager>
	{
	private:
		Dictionary<JG::Type, UniquePtr<IUIView>> mUIViewPool;
		mutable std::shared_mutex mMutex;
	public:
		UIManager();
		~UIManager();
	public:
		// 등록
		template<class UIViewType>
		void RegisterUIView()
		{
			Type type = Type(TypeID<UIViewType>());


			std::lock_guard<std::shared_mutex> lock(mMutex);
			if (mUIViewPool.find(type) != mUIViewPool.end())
			{
				return;
			}
			mUIViewPool[type] = CreateUniquePtr<UIViewType>();
		}

		// UI용 WeakPtr 구현
		template<class UIViewType>
		UIViewType* GetUIView() const
		{
			Type type = Type(TypeID<UIViewType>());

			std::shared_lock<std::shared_mutex> lock(mMutex);
			auto iter = mUIViewPool.find(type);
			if (iter == mUIViewPool.end())
			{
				JG_CORE_ERROR("Not Find UIViewType : {0}", ws2s(type.GetName()));
				return nullptr;
			}
			return static_cast<UIViewType*>(iter->second.get());
		}

		void ForEach(const std::function<void(IUIView*)> action);
	private:
		void OnGUI();
	};
}
