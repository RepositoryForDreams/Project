#include "pch.h"
#include "Renderer.h"
#include "Application.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"

namespace JG
{
	static Dictionary<std::thread::id, bool> gRendererTriggerMap;
	static std::shared_mutex gMutex;

	void RegisterThreadID();
	


	
	void Renderer2D::Begin()
	{
		auto threadID = std::this_thread::get_id();
		RegisterThreadID();
		if(gRendererTriggerMap[threadID] == true)
		{
			JG_CORE_ERROR("{0} : Already Call Begin", std::hash<std::thread::id>()(threadID));
			return;
		}
		gRendererTriggerMap[threadID] = true;





		// Renderer
		// CommandList 준비
	}

	SharedPtr<ITexture> Renderer2D::End()
	{
		auto threadID = std::this_thread::get_id();
		RegisterThreadID();


		if(gRendererTriggerMap[threadID] == false)
		{
			JG_CORE_ERROR("{0} : Not Call Begin", std::hash<std::thread::id>()(threadID));
			return nullptr;
		}
		gRendererTriggerMap[threadID] = false;

		
		

		return nullptr;
	}





	void RegisterThreadID()
	{
		auto threadID = std::this_thread::get_id();
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(gMutex);
			isFind = gRendererTriggerMap.find(threadID) != gRendererTriggerMap.end();
		}

		if (isFind == false)
		{
			{
				std::lock_guard<std::shared_mutex> lock(gMutex);
				gRendererTriggerMap.emplace(threadID, false);
			}
		}
	}
}