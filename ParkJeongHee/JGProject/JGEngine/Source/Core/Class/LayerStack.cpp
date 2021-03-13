#include "pch.h"
#include "LayerStack.h"




namespace JG
{
	LayerStack::~LayerStack()
	{
		for(auto& layer : mLayers)
		{
			layer->OnDetach();
		}
	}

	void LayerStack::PushLayer(const SharedPtr<ILayer>& layer)
	{
		mLayers.insert(mLayers.begin() + mLayerInsertIndex, layer);
		++mLayerInsertIndex;

		layer->OnAttach();
	}

	void LayerStack::PushOverlay(const SharedPtr<ILayer>& layer)
	{
		mLayers.emplace_back(layer);

		layer->OnAttach();
	}

	void LayerStack::PopLayer()
	{
		SharedPtr<ILayer> layer;
		auto it = std::find(mLayers.begin(), mLayers.begin() + mLayerInsertIndex, layer);

		if(it != mLayers.begin() + mLayerInsertIndex)
		{
			layer->OnDetach();
			mLayers.erase(it);
			--mLayerInsertIndex;
		}
		
	}

	void LayerStack::PopOverlay()
	{
		SharedPtr<ILayer> layer;
		auto it = std::find(mLayers.begin() + mLayerInsertIndex, mLayers.end(), layer);

		if(it != mLayers.end())
		{
			layer->OnDetach();
			mLayers.erase(it);
		}
	}

	void LayerStack::ForEach(const std::function<void(ILayer*)>& action)
	{
		for(auto& layer : mLayers)
		{
			action(layer.get());
		}
	}
}
