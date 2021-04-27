#include "pch.h"
#include "SortingLayerSystem.h"

namespace JG
{
	void SortingLayerSystem::AddSortingLayer(const String& name, i64 priority)
	{
		auto layer      = CreateUniquePtr<SortingLayer>();
		layer->Name     = name;
		layer->Priority = priority;

		auto player = layer.get();
		mSortingLayerList.push_back(std::move(layer));
		mSortingLayerByName[layer->Name] = player;
	}
	void SortingLayerSystem::RemoveSortingLayer(const String& name)
	{




	}
	void SortingLayerSystem::BindCamera(const String& layerName, SharedPtr<Camera> camera)
	{
	}
	void SortingLayerSystem::UnBindCamera(const String& layerName, SharedPtr<Camera> camera)
	{
	}
	SortingLayer* SortingLayerSystem::GetSortingLayer(const String& name) const
	{
		return nullptr;
	}
}