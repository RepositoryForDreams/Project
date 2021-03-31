#pragma once
#include "JGCore.h"
#include <Common/Type.h>
#include <shared_mutex>

namespace JG
{
	class IUIView;
	struct MainMenuItem
	{
		String ShortCut;
		std::function<void()> Action;
		std::function<bool()> EnableAction;
	};
	struct MainMenuItemNode
	{
		String Name;
		u64 Priority = 0;
		const MainMenuItemNode* Parent = nullptr;
		UniquePtr<MainMenuItem> MenuItem;
		mutable bool IsOpen = false;
		bool IsRoot = false;
	private:
		friend class UIManager;
		SortedDictionary<u64, List<UniquePtr<MainMenuItemNode>>> ChildNodes;
	};
	class UIManager : public GlobalSingleton<UIManager>
	{
	public:
		static const u64 DEFAULT_PRIORITY = 100;
	private:
		Dictionary<JG::Type, UniquePtr<IUIView>>       mUIViewPool;
		UniquePtr<MainMenuItemNode> mMainMenuItemRootNode;
		mutable std::shared_mutex   mMutex;
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
		void RegisterMainMenuRootNode(const String& menuName, u64 priority);
		void RegisterMainMenuItem(const String& menuPath, u64 priority,  const std::function<void()>& action, const std::function<bool()> enableAction);
		void ForEach(const std::function<void(IUIView*)> action);
		void ForEach(
			const std::function<void(const MainMenuItemNode*)>& beginAction,
			const std::function<void(const MainMenuItemNode*)>& endAction);
		bool IsMainMenuRootNode(const MainMenuItemNode* node) const;
	private:
		void OnGUI();

		MainMenuItemNode* FindMainMenuItemNode(MainMenuItemNode* parentNode, const String& menuName);
		MainMenuItemNode* RegisterMainMenuNode(MainMenuItemNode* parentNode, const String& menuName, u64 priority);
		void ExtractPathAndShortcut(const String& menuPath, String* out_path, String* out_shortCut);
	};
}
