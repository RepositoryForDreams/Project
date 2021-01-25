#include "pch.h"
#include "DirectX12API.h"
#include "DirectX12FrameBuffer.h"
#include "Utill/DirectX12Helper.h"
#include "Utill/DescriptorAllocator.h"

namespace JG
{
	static ComPtr<IDXGIFactory4> gFactory;
	static ComPtr<ID3D12Device>  gDevice;
	static UniquePtr<DescriptorAllocator> gCSUAllocator;
	static UniquePtr<DescriptorAllocator> gRTVAllocator;
	static UniquePtr<DescriptorAllocator> gDSVAllocator;
	static std::vector<UniquePtr<DirectX12FrameBuffer>> gFrameBuffers;
	static u64 mFrameBufferIndex = 0;
	
	bool DirectX12API::Create()
	{
		JG_CORE_INFO("DirectX12 Init Start");

		//
		gFactory = CreateDXGIFactory();
		if(gFactory)
		{
			JG_CORE_INFO("Success Create DXGIFactroy");
		}
		else
		{
			JG_CORE_CRITICAL("Failed Create DXGIFactory");
			return false;
		}
		DXGI_ADAPTER_DESC1 adapterDesc = {};
		gDevice = CreateD3DDevice(gFactory, false, &adapterDesc);

		if(gDevice)
		{
			JG_CORE_INFO("Success Create D3D12Device");
			JG_CORE_TRANCE(ws2s(String(TT("Description : ")) + adapterDesc.Description));
			JG_CORE_TRANCE("VideoMemory : {0}  MB", adapterDesc.DedicatedVideoMemory / 1024 / 1024);
		}
		else
		{
			JG_CORE_CRITICAL("Failed Create D3D12Device");
			return false;
		}

		// NOTE
		// DescriptiorAllcator 持失
		gCSUAllocator = CreateUniquePtr<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		gRTVAllocator = CreateUniquePtr<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		gRTVAllocator = CreateUniquePtr<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		
		// FrameBuffer 持失

		
		JG_CORE_INFO("DirectX12 Init End");
		return true;
	}
	void DirectX12API::Destroy()
	{

		gDevice.Reset();
		gFactory.Reset();
	}
	EGraphicsAPI DirectX12API::GetAPI() const
	{
		return EGraphicsAPI::DirectX12;
	}

	IDXGIFactory4* DirectX12API::GetDXGIFactory()
	{
		return gFactory.Get();
	}

	ID3D12Device* DirectX12API::GetD3DDevice()
	{
		return gDevice.Get();
	}

	u64 DirectX12API::GetFrameBufferIndex()
	{
		return mFrameBufferIndex;
	}
}
