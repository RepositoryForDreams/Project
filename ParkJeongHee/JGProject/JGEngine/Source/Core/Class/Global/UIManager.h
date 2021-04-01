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
		enum ENodeType
		{
			MainMenu,
			Context,
		};
		String    Name;
		u64       Priority = 0;
		bool      IsSperator = false;
		ENodeType NodeType = ENodeType::MainMenu;


		const MenuItemNode* Parent = nullptr;
		UniquePtr<MenuItem> MenuItem;
		mutable bool IsOpen = false;

	private:
		friend class UIManager;
		SortedDictionary<u64, List<UniquePtr<MenuItemNode>>> ChildNodes;
	};
	class UIManager : public GlobalSingleton<UIManager>
	{
	public:
		static const u64 DEFAULT_PRIORITY = 1000;
		static const u64 SPERATOR_PRIORITY_DELTA = 10;
		static const wchar CTRL_SHORTCUT_TOKEN  = TT('%');
		static const wchar SHIFT_SHORTCUT_TOKEN = TT('#');
		static const wchar ALT_SHORTCUT_TOKEN   = TT('&');
	private:
		Dictionary<JG::Type, UniquePtr<IUIView>>       mUIViewPool;
		UniquePtr<MenuItemNode> mMainMenuItemRootNode;
		mutable std::shared_mutex   mMutex;
	public:
		UIManager();
		~UIManager();
	public:
		// ���
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

		// UI�� WeakPtr ����
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
		void RegisterMenuItem(const String& menuPath, u64 priority,  const std::function<void()>& action, const std::function<bool()> enableAction);
		void ForEach(const std::function<void(IUIView*)> action);
		void ForEach(
			MenuItemNode::ENodeType nodeType,
			const std::function<void(const MenuItemNode*)>& beginAction,
			const std::function<void(const MenuItemNode*)>& endAction);
	private:
		void OnGUI();
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
