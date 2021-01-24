#include "pch.h"
#include "DirectX12API.h"


namespace JG
{
	bool DirectX12API::Create()
	{
		JG_CORE_INFO("DirectX12 Init Start");

		//


		
		JG_CORE_INFO("DirectX12 Init End");
		return true;
	}
	void DirectX12API::Destroy()
	{
		
	}
	EGraphicsAPI DirectX12API::GetAPI() const
	{
		return EGraphicsAPI::DirectX12;
	}
}