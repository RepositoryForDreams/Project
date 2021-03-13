#pragma once

#include "Common/Define.h"
#include "Layer.h"
#include <vector>
#include <functional>


namespace JG
{
	class LayerStack
	{
	private:
		List<SharedPtr<ILayer>> mLayers;
		u32 mLayerInsertIndex = 0;
	public:
		LayerStack() = default;
		~LayerStack();
	public:
		void PushLayer(const SharedPtr<ILayer>& layer);
		void PushOverlay(const SharedPtr<ILayer>& layer);


		void PopLayer();
		void PopOverlay();
		
		void ForEach(const std::function<void(ILayer*)>& action);
	};
}

