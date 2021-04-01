#include "pch.h"
#include "UIManager.h"
#include "Class/UI/UIView.h"

namespace JG
{
	UIManager::UIManager()
	{
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::OnGUI,
			[&]() -> EScheduleResult
		{
			OnGUI();
			return EScheduleResult::Continue;
		});

	}
	UIManager::~UIManager()
	{
		for (auto& _pair : mUIViewPool)
		{
			if (_pair.second->IsOpen())
			{
				_pair.second->Close();
			}
		}
		mUIViewPool.clear();
	}
	void UIManager::RegisterMenuItem(const String& menuPath, u64 priority, const std::function<void()>& action, const std::function<bool()> enableAction)
	{
		if (mMainMenuItemRootNode == nullptr)
		{
			mMainMenuItemRootNode = CreateUniquePtr<MenuItemNode>();
			mMainMenuItemRootNode->Name = TT("Root");
			mMainMenuItemRootNode->IsOpen = true;
			mMainMenuItemRootNode->NodeType = MenuItemNode::ENodeType::MainMenu;
		}
		RegisterMenuItem(mMainMenuItemRootNode.get(), menuPath, priority, action, enableAction);
	}
	void UIManager::ForEach(const std::function<void(IUIView*)> action)
	{
		for (auto& _pair : mUIViewPool)
		{
			action(_pair.second.get());
		}
	}

	void UIManager::ForEach(
		MenuItemNode::ENodeType nodeType,
		const std::function<void(const MenuItemNode*)>& beginAction,
		const std::function<void(const MenuItemNode*)>& endAction)
	{
		switch (nodeType)
		{
		case MenuItemNode::ENodeType::MainMenu:
			ForEach(mMainMenuItemRootNode.get(), beginAction, endAction);
			break;
		case MenuItemNode::ENodeType::Context:
			break;
		}
	}



	void UIManager::OnGUI()
	{
		for (auto& _pair : mUIViewPool)
		{
			if (_pair.second->IsOpen())
			{
				_pair.second->OnGUI();
			}
		}
	}
	void UIManager::ForEach(MenuItemNode* rootNode, const std::function<void(const MenuItemNode*)>& beginAction, const std::function<void(const MenuItemNode*)>& endAction)
	{
		if (rootNode == nullptr)
		{
			return;
		}
		using NodeIterator = SortedDictionary<u64, List<UniquePtr<MenuItemNode>>>::iterator;
		struct NodeHistroy
		{
			MenuItemNode* Node = nullptr;
			NodeIterator Iterater;
			u64 Index = 0;
		};


		Stack<NodeHistroy> nodeStack;
		NodeHistroy curr;
		curr.Node = rootNode;
		curr.Iterater = curr.Node->ChildNodes.begin();
		curr.Index = 0;

		while (true)
		{
			if (curr.Node->MenuItem == nullptr)
			{
				auto& iter = curr.Iterater;
				auto index = curr.Index;
				if (iter != curr.Node->ChildNodes.end())
				{
					auto nodeCnt = iter->second.size();
					if (index < nodeCnt)
					{
						auto pushHistory = curr;

						curr.Node = iter->second[index].get();
						curr.Iterater = curr.Node->ChildNodes.begin();
						curr.Index = 0;
						// Begin
						if (index != 0)
						{
							curr.Node->IsSperator = false;
						}
						beginAction(curr.Node);
						nodeStack.push(pushHistory);
						continue;
					}
					else
					{
						u64 prevPriority = curr.Iterater->first;
						curr.Iterater++;
						if (curr.Iterater != curr.Node->ChildNodes.end())
						{
							u64 nextPriority = curr.Iterater->first;
							u64 delta = nextPriority - prevPriority;
							if (delta > SPERATOR_PRIORITY_DELTA)
							{
								curr.Iterater->second[0]->IsSperator = true;
							}
						}
						curr.Index = 0;
						continue;
					}
				}
				else
				{
					if (curr.Node != rootNode)
					{
						endAction(curr.Node);
					}
					if (nodeStack.empty() == false)
					{
						curr = nodeStack.top(); nodeStack.pop();
						curr.Index += 1;
						continue;
					}
					else
					{
						break;
					}
				}

			}
			else
			{
				if (nodeStack.empty() == false)
				{
					curr = nodeStack.top(); nodeStack.pop();
					curr.Index += 1;
					continue;
				}
				else
				{
					break;
				}
			}


		}
	}
	void UIManager::RegisterMenuItem(MenuItemNode* rootNode, const String& menuPath, u64 priority, const std::function<void()>& action, const std::function<bool()> enableAction)
	{
		String path;
		String shortCut;

		ExtractPathAndShortcut(menuPath, &path, &shortCut);

		// Path
		u64  pos = path.find_first_of(TT("/"));
		MenuItemNode* currRootNode = nullptr;
		if (pos == String::npos)
		{
			RegisterMenuNode(rootNode, menuPath, priority);
			return;
		}
		else
		{
			auto rootName = path.substr(0, pos);
			currRootNode = FindMenuItemNode(rootNode, rootName, priority);
		}

		path = path.substr(pos + 1, path.length() - pos);
		u64    start = 0;
		while (true)
		{
			u64  end = path.find_first_of(TT("/"));
			if (end == String::npos)
			{
				currRootNode = RegisterMenuNode(currRootNode, path, priority);
				currRootNode->MenuItem = CreateUniquePtr<MenuItem>();
				currRootNode->MenuItem->Action = action;
				currRootNode->MenuItem->EnableAction = enableAction;
				currRootNode->MenuItem->ShortCut = shortCut;
				break;
			}
			else
			{
				auto menu = path.substr(start, end - start);
				path = path.substr(end + 1, path.length() - end);
				currRootNode = FindMenuItemNode(currRootNode, menu, priority);
			}
		}
	}

	MenuItemNode* UIManager::FindMenuItemNode(MenuItemNode* parentNode, const String& menuName, u64 default_priority)
	{
		for (auto& _pair : parentNode->ChildNodes)
		{
			for (auto& item : _pair.second)
			{
				if (item->MenuItem == nullptr && item->Name == menuName)
				{
					return item.get();
				}
			}
		}
		return RegisterMenuNode(parentNode, menuName, default_priority);
	}
	MenuItemNode* UIManager::RegisterMenuNode(MenuItemNode* parentNode, const String& menuName, u64 priority)
	{
		auto menuItem = CreateUniquePtr<MenuItemNode>();
		auto result = menuItem.get();
		menuItem->Name     = menuName;
		menuItem->Parent   = parentNode;
		menuItem->Priority = priority;
		menuItem->NodeType = parentNode->NodeType;
		parentNode->ChildNodes[priority].push_back(std::move(menuItem));
		return result;
	}
	void UIManager::ExtractPathAndShortcut(const String& menuPath, String* out_path, String* out_shortCut)
	{
		u64 midPos = menuPath.find_first_of(TT(" "));


		if (midPos == String::npos)
		{
			if (out_path)
			{
				*out_path = menuPath;
			}
		}
		else
		{
			if (out_path)
			{
				*out_path = menuPath.substr(0, midPos);
			}
			if (out_shortCut)
			{
				auto short_cut = menuPath.substr(midPos + 1, menuPath.length() - midPos);
				short_cut = ReplaceAll(short_cut, TT(" "), TT(""));

				wchar splitToken = TT('_');
				u64 splitPos = short_cut.find_first_of(splitToken);
				if (splitPos != String::npos)
				{
					String token = short_cut.substr(0, splitPos);
					short_cut    = short_cut.substr(splitPos + 1);

					if (token.find(CTRL_SHORTCUT_TOKEN) != String::npos)
					{
						*out_shortCut += TT("Ctrl + ");
					}
					if (token.find(SHIFT_SHORTCUT_TOKEN) != String::npos)
					{
						*out_shortCut += TT("Shift + ");
					}
					if (token.find(ALT_SHORTCUT_TOKEN) != String::npos)
					{
						*out_shortCut += TT("Alt + ");
					}


					if (short_cut.length() != 0 && out_shortCut->length() != 0)
					{
						while (true)
						{
							u64 pos = short_cut.find_first_of(splitToken);
							if (pos == String::npos)
							{
								*out_shortCut += short_cut;
								break;
							}
							else
							{
								*out_shortCut += short_cut.substr(0, pos) + TT(" + ");
								short_cut = short_cut.substr(pos + 1);
							}
						}
					}
					else
					{
						out_shortCut->clear();
					}
				}
				
			}
		}


	}
}

