#include "GameNode.h"
#include "GameNode.h"
#include "pch.h"
#include "GameNode.h"
#include "Components/Transform.h"
namespace JG
{
	void GameNode::Start()
	{
		GameObject::Start();
	}
	void GameNode::Destory()
	{
		GameObject::Destory();
		SetParent(nullptr);
	}
	void GameNode::Update()
	{
		if (mIsRunStart == false)
		{
			mIsRunStart = true;
			Start();
		}
		for (auto& com : mComponents)
		{
			if (com->IsActive())
			{
				if (com->mIsRunStart == false)
				{
					com->mIsRunStart = true;
					com->Start();
				}
				com->Update();
			}
		}

		for (auto& child : mChilds)
		{
			if (child->IsActive())
			{
				child->Update();
			}
		}
	}
	void GameNode::LateUpdate()
	{
		for (auto& com : mComponents)
		{
			if (com->IsActive())
			{
				com->LateUpdate();
			}
		}

		for (auto& child : mChilds)
		{
			if (child->IsActive())
			{
				child->LateUpdate();
			}
		}
	}
	GameNode::GameNode()
	{
		mTransform = AddComponent<Transform>();
	}
	GameNode::GameNode(GameWorld* gameWorld) : GameNode()
	{
		mGameWorld = gameWorld;
	}
	void GameNode::OnInspectorGUI() 
	{
		const float width = ImGui::GetWindowWidth();
		const float wpadding = ImGui::GetStyle().FramePadding.x;

		ImGui::Dummy(ImVec2(0, 1.0f));

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Name"); ImGui::SameLine();

		char objName[128] = { 0, };
		strcpy(objName, ws2s(GetName()).c_str());
		const float input_width = width * 0.4f;
		ImGui::SetNextItemWidth(input_width);
		if (ImGui::InputText(("##GameNode InputName"), objName, 128))
		{
			SetName(s2ws(objName));
		}






		const float combo_width = width * 0.2f;
		ImGui::SameLine(width - ImGui::CalcTextSize("Layer").x - wpadding * 2 - combo_width);
		ImGui::Text("Layer"); ImGui::SameLine();
		ImGui::SetNextItemWidth(combo_width);
		ImGui::SameLine(width - combo_width - wpadding);
		if (ImGui::BeginCombo("##Layer Combo Box", ws2s(mTargetLayer).c_str()))
		{
			GameLayerManager::GetInstance().ForEach([&](const String& layerName)
			{
				bool _bool = false;
				if (ImGui::Selectable(ws2s(layerName).c_str(), &_bool))
				{
					SetLayer(layerName);
				}
			});
			ImGui::EndCombo();
		}


		ImGui::Dummy(ImVec2(0, 1.0f));
		ImGui::Separator();
		List<GameComponent*> removeComList;
		for (auto& com : mComponents)
		{
			bool is_open = true;
			ImGui::Spacing();
			String id = com->GetName() + TT("##") + std::to_wstring((ptraddr)com);
			if (ImGui::CollapsingHeader(
				ws2s(id).c_str(), &is_open, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader) == true)
			{
				com->OnInspectorGUI();
			}

			if (is_open == false)
			{
				removeComList.push_back(com);
			}
		}

		for (auto& com : removeComList)
		{
			Destroy(com);
		}
	}
	SharedPtr<IRenderItem> GameNode::PushRenderItem()
	{
		for (auto& com : mComponents)
		{
			if (com->IsActive())
			{
				auto item = com->PushRenderItem();
				if (item != nullptr)
				{
					RequestPushRenderItemEvent e;
					e.RenderItem = item;
					SendEvent(e);
				}
			}
			
		}
		for (auto& child : mChilds)
		{
			if (child->IsActive())
			{
				auto item = child->PushRenderItem();
				if (item != nullptr)
				{
					RequestPushRenderItemEvent e;
					e.RenderItem = item;
					SendEvent(e);
				}
			}
		}

		return nullptr;
	}
	GameNode* GameNode::AddNode(const String& name)
	{
		auto obj = GameObjectFactory::GetInstance().CreateObject<GameNode>();
		obj->SetName(name);
		obj->SetParent(this);
		obj->mGameWorld = mGameWorld;
		obj->Awake();
		return obj;
	}
	void GameNode::AddComponent(const Type& type)
	{

		bool isComponent = GameObjectFactory::GetInstance().IsGameComponent(type);
		if (isComponent == false)
		{
			JG_CORE_ERROR("This Type({0}) is not Component Category.", type.GetName());
			return;
		}
		auto obj = GameObjectFactory::GetInstance().CreateObjectByType(type);
		if (obj == nullptr)
		{
			JG_CORE_ERROR("This Type({0}) is not registered.", type.GetName());
			return;
		}
		auto com = static_cast<GameComponent*>(obj);
		com->mOwnerNode = this;
		com->mGameWorld = mGameWorld;
		mComponents.push_back(com);
		com->Awake();
	}
	void GameNode::Destroy(GameNode* node)
	{
		node->DestroyRecursive();
	}
	void GameNode::Destroy(GameComponent* component)
	{
		if (mTransform == component)
		{
			return;
		}
		auto& begin = mComponents.begin();
		auto& end   = mComponents.end();
		mComponents.erase(std::remove(begin, end, component), mComponents.end());
		GameObjectFactory::GetInstance().DestroyObject(component);
	}
	void GameNode::ForEach(const std::function<void(GameNode*)>& action)
	{
		for (auto& node : mChilds)
		{
			action(node);
		}
	}
	void GameNode::SendChangeData(const ChangeData& data, EChangeDataFlags flags)
	{
		for (auto& com : mComponents)
		{
			if (com == data.Object)
			{
				continue;
			}
			com->OnChange(data);
		}

		for (auto& node : mChilds)
		{
			if (node == data.Object)
			{
				continue;
			}
			node->OnChange(data);
			node->SendChangeData(data);
		}

	}
	GameNode* GameNode::FindNode(const String& name) const
	{
		for (auto& node : mChilds)
		{
			if (node->GetName() == name)
			{
				return node;
			}
		}
		return nullptr;
	}
	GameNode* GameNode::FindNode(u32 index) const
	{
		if (mChilds.size() <= index)
		{
			return nullptr;
		}
		return mChilds[index];
	}
	u64 GameNode::GetChildCount() const
	{
		return mChilds.size();
	}
	void GameNode::SetParent(GameNode* node)
	{
		bool isDiff = mParent != node;
		if (isDiff == false)
		{
			return;
		}
		if (mParent != nullptr)
		{
			auto& begin = mParent->mChilds.begin();
			auto& end   = mParent->mChilds.end();
			mParent->mChilds.erase(std::remove(begin, end, this), mParent->mChilds.end());
			mParent = nullptr;
		}
		if (node != nullptr)
		{
			mParent = node;
			mParent->mChilds.push_back(this);
		}
	}
	GameNode* GameNode::GetParent() const
	{
		return mParent;
	}
	void GameNode::SetLayer(const String& layer)
	{
		if (GameLayerManager::GetInstance().IsRegisterLayer(layer) == false)
		{
			mTargetLayer = GameLayer::DEFAULT_LAYER;
		}
		else
		{
			mTargetLayer = layer;
		}
	}
	const String& GameNode::GetLayer()
	{
		return mTargetLayer;
	}
	bool GameNode::IsActive() const
	{
		return mIsActive;
	}
	void GameNode::DestroyRecursive()
	{
		for (auto& child : mChilds)
		{
			child->DestroyRecursive();
		}
		for (auto& com : mComponents)
		{
			GameObjectFactory::GetInstance().DestroyObject(com);
		}
		for (auto& child : mChilds)
		{
			GameObjectFactory::GetInstance().DestroyObject(child);
		}
		GameObjectFactory::GetInstance().DestroyObject(this);
	}
}