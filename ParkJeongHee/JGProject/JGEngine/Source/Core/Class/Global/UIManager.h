#pragma once
#include "JGCore.h"
#include <Common/Type.h>
#include <shared_mutex>

namespace JG
{
	class IUIView;

	struct MenuItem
	{
		String ShortCut;
		std::function<void()> Action;
		std::function<bool()> EnableAction;
	};

	struct MenuItemNode
	{
		String    Name;
		u64       Priority = 0;
		bool      IsSperator = false;

		const MenuItemNode* Parent = nullptr;
		UniquePtr<MenuItem> MenuItem;
		mutable bool IsOpen = false;

	private:
		friend class UIManager;
		SortedDictionary<u64, List<UniquePtr<MenuItemNode>>> ChildNodes;
	};
	class UIManager : public GlobalSingleton<UIManager>
	{
		friend class Application;
	public:
		static const u64 DEFAULT_PRIORITY = 1000;
		static const u64 SPERATOR_PRIORITY_DELTA = 10;
		static const wchar CTRL_SHORTCUT_TOKEN  = TT('%');
		static const wchar SHIFT_SHORTCUT_TOKEN = TT('#');
		static const wchar ALT_SHORTCUT_TOKEN   = TT('&');
	private:
		Dictionary<JG::Type, UniquePtr<IUIView>>      mUIViewPool;
		Dictionary<JG::Type, UniquePtr<MenuItemNode>> mUIViewContextMenu;
		UniquePtr<MenuItemNode> mMainMenuItemRootNode;
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
			mUIViewPool[type]        = CreateUniquePtr<UIViewType>();
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
		void RegisterMainMenuItem(const String& menuPath, u64 priority,  const std::function<void()>& action, const std::function<bool()> enableAction);
		void RegisterContextMenuItem(const Type& type, const String& menuPath, u64 priority, const std::function<void()>& action, const std::function<bool()> enableAction);
		void ForEach(const std::function<void(IUIView*)> action);
		void ForEach(
			const std::function<void(const MenuItemNode*)>& beginAction,
			const std::function<void(const MenuItemNode*)>& endAction);
		void ForEach(
			const Type& type,
			const std::function<void(const MenuItemNode*)>& beginAction,
			const std::function<void(const MenuItemNode*)>& endAction);
	private:
		void OnGUI();
		void OnEvent(IEvent& e);
		void ForEach(
			MenuItemNode* rootNode,
			const std::function<void(const MenuItemNode*)>& beginAction,
			const std::function<void(const MenuItemNode*)>& endAction);

		void RegisterMenuItem(MenuItemNode* rootNode, const String& menuPath, u64 priority, const std::function<void()>& action, const std::function<bool()> enableAction);
		MenuItemNode* FindMenuItemNode(MenuItemNode* parentNode, const String& menuName, u64 default_priority = DEFAULT_PRIORITY);
		MenuItemNode* RegisterMenuNode(MenuItemNode* parentNode, const String& menuName, u64 priority);
		void ExtractPathAndShortcut(const String& menuPath, String* out_path, String* out_shortCut);
	};
}
