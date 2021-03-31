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
	void UIManager::RegisterMainMenuRootNode(const String& menuName, u64 priority)
	{
		if (mMainMenuItemRootNode == nullptr)
		{
			mMainMenuItemRootNode = CreateUniquePtr<MainMenuItemNode>();
			mMainMenuItemRootNode->Name = TT("Root");
			mMainMenuItemRootNode->IsOpen = true;
			mMainMenuItemRootNode->IsRoot = true;
		}
		auto node = RegisterMainMenuNode(mMainMenuItemRootNode.get(), menuName, priority);
		node->IsRoot = true;
	}
	void UIManager::RegisterMainMenuItem(const String& menuPath, u64 priority, const std::function<void()>& action, const std::function<bool()> enableAction)
	{
		String path;
		String shortCut;

		ExtractPathAndShortcut(menuPath, &path, &shortCut);

		// Path
		u64  pos      = path.find_first_of(TT("/"));
		auto rootName = path.substr(0, pos);
		MainMenuItemNode* currRootNode = FindMainMenuItemNode(mMainMenuItemRootNode.get(), rootName);

		

		path = path.substr(pos + 1, path.length() - pos);
		u64    start = 0;
		while (true)
		{
			u64  end  = path.find_first_of(TT("/"));
			if (end == String::npos)
			{
				currRootNode = RegisterMainMenuNode(currRootNode, path, priority);
				currRootNode->MenuItem = CreateUniquePtr<MainMenuItem>();
				currRootNode->MenuItem->Action       = action;
				currRootNode->MenuItem->EnableAction = enableAction;
				currRootNode->MenuItem->ShortCut     = shortCut;
				break;
			}
			else
			{
				auto menu = path.substr(start, end - start);
				path = path.substr(end + 1, path.length() - end);
				currRootNode = FindMainMenuItemNode(currRootNode, menu);
			}
		}
	}
	void UIManager::ForEach(const std::function<void(IUIView*)> action)
	{
		for (auto& _pair : mUIViewPool)
		{
			action(_pair.second.get());
		}
	}

	void UIManager::ForEach(
		const std::function<void(const MainMenuItemNode*)>& beginAction,
		const std::function<void(const MainMenuItemNode*)>& endAction)
	{
		using NodeIterator = SortedDictionary<u64, List<UniquePtr<MainMenuItemNode>>>::iterator;
		struct NodeHistroy
		{
			MainMenuItemNode* Node = nullptr;
			NodeIterator Iterater;
			u64 Index    = 0;
		};

		
		Stack<NodeHistroy> nodeStack;
		NodeHistroy curr;
		curr.Node     = mMainMenuItemRootNode.get();
		curr.Iterater = curr.Node->ChildNodes.begin();
		curr.Index    = 0;

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

						curr.Node     = iter->second[index].get();
						curr.Iterater = curr.Node->ChildNodes.begin();
						curr.Index    = 0;
						// Begin
						beginAction(curr.Node);
						nodeStack.push(pushHistory);
						continue;
					}
					else
					{
						curr.Iterater++;
						curr.Index = 0;
						continue;
					}
				}
				else 
				{
					if (curr.Node != mMainMenuItemRootNode.get())
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

	bool UIManager::IsMainMenuRootNode(const MainMenuItemNode* node) const
	{
		return node->Parent == mMainMenuItemRootNode.get();
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
	MainMenuItemNode* UIManager::FindMainMenuItemNode(MainMenuItemNode* parentNode, const String& menuName)
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
		return RegisterMainMenuNode(parentNode, menuName, DEFAULT_PRIORITY);
	}
	MainMenuItemNode* UIManager::RegisterMainMenuNode(MainMenuItemNode* parentNode, const String& menuName, u64 priority)
	{
		auto menuItem = CreateUniquePtr<MainMenuItemNode>();
		auto result = menuItem.get();
		menuItem->Name     = menuName;
		menuItem->Parent   = parentNode;
		menuItem->Priority = priority;
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
				*out_shortCut = menuPath.substr(midPos + 1, menuPath.length() - midPos);
			}
		}


	}
}

