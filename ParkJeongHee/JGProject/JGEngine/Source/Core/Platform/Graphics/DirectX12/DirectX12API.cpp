#include "pch.h"
#include "DirectX12API.h"
#include "DirectX12FrameBuffer.h"

#include "Utill/DirectX12Helper.h"
#include "Utill/DescriptorAllocator.h"
#include "Utill/CommandQueue.h"
#include "Utill/RootSignature.h"
namespace JG
{
	static ComPtr<IDXGIFactory4> gFactory;
	static ComPtr<ID3D12Device>  gDevice;
	static UniquePtr<DescriptorAllocator> gCSUAllocator;
	static UniquePtr<DescriptorAllocator> gRTVAllocator;
	static UniquePtr<DescriptorAllocator> gDSVAllocator;

	static UniquePtr<CommandQueue> gGraphicsCommandQueue;
	static UniquePtr<CommandQueue> gComputeCommandQueue;
	static UniquePtr<CommandQueue> gCopyCommandQueue;
	static std::vector<UniquePtr<DirectX12FrameBuffer>> gFrameBuffers;
	static const u64 gFrameBufferCount = 3;
	static u64 gFrameBufferIndex = 0;

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
		return gFrameBufferIndex;
	}


	bool DirectX12API::Create()
	{
		JG_CORE_INFO("DirectX12 Init Start");

		//
		gFactory = CreateDXGIFactory();
		if (gFactory)
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

		if (gDevice)
		{
			JG_CORE_INFO("Success Create D3D12Device");
			JG_CORE_TRACE(ws2s(String(TT("Description : ")) + adapterDesc.Description));
			JG_CORE_TRACE("VideoMemory : {0}  MB", adapterDesc.DedicatedVideoMemory / 1024 / 1024);
		}
		else
		{
			JG_CORE_CRITICAL("Failed Create D3D12Device");
			return false;
		}

		// NOTE
		// DescriptiorAllcator 持失
		JG_CORE_INFO("Create DescriptorAllocator...");
		gCSUAllocator = CreateUniquePtr<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		gRTVAllocator = CreateUniquePtr<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		gRTVAllocator = CreateUniquePtr<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);


		JG_CORE_INFO("Create CommandQueue...");
		gGraphicsCommandQueue = CreateUniquePtr<CommandQueue>(gFrameBufferCount, D3D12_COMMAND_LIST_TYPE_DIRECT);
		gComputeCommandQueue  = CreateUniquePtr<CommandQueue>(gFrameBufferCount, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		gCopyCommandQueue     = CreateUniquePtr<CommandQueue>(gFrameBufferCount, D3D12_COMMAND_LIST_TYPE_COPY);

		JG_CORE_INFO("Create FrameBuffer...");
		for (u64 i = 0; i < gFrameBufferCount; ++i)
		{
			// FrameBuffer 持失
		}
		// FrameBuffer 持失
		JG_CORE_INFO("DirectX12 Init End");
		return true;
	}
	void DirectX12API::Destroy()
	{
		Flush();

		RootSignature::ClearCache();

		gCSUAllocator.reset();
		gRTVAllocator.reset();
		gDSVAllocator.reset();

		gGraphicsCommandQueue.reset();
		gComputeCommandQueue.reset();
		gCopyCommandQueue.reset();

		gFrameBuffers.clear();

		gDevice.Reset();
		gFactory.Reset();
	}


	void DirectX12API::Begin()
	{
		gGraphicsCommandQueue->Begin();
		gComputeCommandQueue->Begin();
		gCopyCommandQueue->Begin();
	}
	void DirectX12API::End()
	{
		// TODO
		// FrameBuffer Update

		gGraphicsCommandQueue->End();
		gComputeCommandQueue->End();
		gCopyCommandQueue->End();


		// TODO
		// SwapBuffer

		gFrameBufferIndex = (gFrameBufferIndex + 1) % gFrameBufferCount;
		gCSUAllocator->UpdatePage();
		gRTVAllocator->UpdatePage();
		gRTVAllocator->UpdatePage();
	}
	void DirectX12API::Flush()
	{
		gGraphicsCommandQueue->Flush();
		gComputeCommandQueue->Flush();
		gCopyCommandQueue->Flush();
	}
}
