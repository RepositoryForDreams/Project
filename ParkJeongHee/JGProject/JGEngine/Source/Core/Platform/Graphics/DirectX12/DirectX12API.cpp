#include "pch.h"
#include "DirectX12API.h"
#include "DirectX12FrameBuffer.h"
#include "DirectX12Resource.h"
#include "DirectX12Shader.h"
#include "Utill/DirectX12Helper.h"
#include "Utill/DescriptorAllocator.h"
#include "Utill/CommandQueue.h"
#include "Utill/RootSignature.h"
#include "Utill/PipelineState.h"
#include "Utill/CommandList.h"
#include "Utill/ResourceStateTracker.h"

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



	static Dictionary<handle, SharedPtr<DirectX12FrameBuffer>> gFrameBuffers;
	static const u64 gFrameBufferCount = 3;
	static u64       gFrameBufferIndex = 0;


	static Dictionary<std::thread::id, List<GraphicsCommandList*>> gGraphicsCommandLists;
	static Dictionary<std::thread::id, List<ComputeCommandList*>>  gComputeCommandLists;
	static Dictionary<std::thread::id, List<CopyCommandList*>>     gCopyCommandLists;

	static std::shared_mutex gGraphicsCommandListMutex;
	static std::shared_mutex gComputeCommandListMutex;
	static std::shared_mutex gCopyCommandListMutex;


	// thread id = rendererID, commandList 획득  =  FrameBufferf
	// mutex = 
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
	CommandQueue* DirectX12API::GetGraphicsCommandQueue()
	{
		return gGraphicsCommandQueue.get();
	}
	CommandQueue* DirectX12API::GetComputeCommandQueue()
	{
		return gComputeCommandQueue.get();
	}
	CommandQueue* DirectX12API::GetCopyCommandQueue()
	{
		return gCopyCommandQueue.get();
	}
	u64	DirectX12API::GetFrameBufferCount()
	{
		return gFrameBufferCount;
	}
	u64 DirectX12API::GetFrameBufferIndex()
	{
		return gFrameBufferIndex;
	}

	DescriptorAllocation DirectX12API::RTVAllocate()
	{
		return gRTVAllocator->Allocate();
	}
	DescriptorAllocation DirectX12API::DSVAllocate()
	{
		return gDSVAllocator->Allocate();
	}
	DescriptorAllocation DirectX12API::CSUAllocate()
	{
		return gCSUAllocator->Allocate();
	}
	// TODO
	// Priority -> RendererID
	// Renderer Begin 호출시 RendererID를 발급 받고
	// Bind로 넘길시 RendererID 로
	// RendererID 별 thread 별 commandList 생성
	GraphicsCommandList* DirectX12API::GetGraphicsCommandList(i32 priority)
	{
		List<GraphicsCommandList*>* pCmdLists = nullptr;
		GraphicsCommandList* result = nullptr;
		u64 bufferIndex = GetFrameBufferIndex();
		auto threadID = std::this_thread::get_id();
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(gGraphicsCommandListMutex);
			isFind = gGraphicsCommandLists.find(threadID) != gGraphicsCommandLists.end();
			if (isFind)
			{
				pCmdLists = &gGraphicsCommandLists[threadID];
			}
		}

		if (isFind == false)
		{
			std::lock_guard<std::shared_mutex> lock(gGraphicsCommandListMutex);
			gGraphicsCommandLists[threadID].resize(GetFrameBufferCount(), nullptr);
			pCmdLists = &gGraphicsCommandLists[threadID];
		}

		if (pCmdLists != nullptr)
		{
			if ((*pCmdLists)[bufferIndex] == nullptr)
			{
				std::lock_guard<std::shared_mutex> lock(gGraphicsCommandListMutex);
				(*pCmdLists)[bufferIndex] = static_cast<GraphicsCommandList*>(gGraphicsCommandQueue->RequestCommandList(priority));
			}

			result = (*pCmdLists)[bufferIndex];
			return result;
		}
		return nullptr;
	}
	ComputeCommandList* DirectX12API::GetComputeCommandList(i32 priority)
	{
		List<ComputeCommandList*>* pCmdLists = nullptr;
		ComputeCommandList* result = nullptr;
		u64 bufferIndex = GetFrameBufferIndex();
		auto threadID = std::this_thread::get_id();
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(gComputeCommandListMutex);
			isFind = gComputeCommandLists.find(threadID) != gComputeCommandLists.end();
			if (isFind)
			{
				pCmdLists = &gComputeCommandLists[threadID];
			}
		}

		if (isFind == false)
		{
			std::lock_guard<std::shared_mutex> lock(gComputeCommandListMutex);
			gComputeCommandLists[threadID].resize(GetFrameBufferCount(), nullptr);
			pCmdLists = &gComputeCommandLists[threadID];
		}

		if (pCmdLists != nullptr)
		{
			if ((*pCmdLists)[bufferIndex] == nullptr)
			{
				std::lock_guard<std::shared_mutex> lock(gComputeCommandListMutex);
				(*pCmdLists)[bufferIndex] = static_cast<ComputeCommandList*>(gComputeCommandQueue->RequestCommandList(priority));
			}

			result = (*pCmdLists)[bufferIndex];
			return result;
		}
		return nullptr;
	}
	CopyCommandList* DirectX12API::GetCopyCommandList(i32 priority)
	{
		List<CopyCommandList*>* pCmdLists = nullptr;
		CopyCommandList* result = nullptr;
		u64 bufferIndex = GetFrameBufferIndex();
		auto threadID = std::this_thread::get_id();
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(gCopyCommandListMutex);
			isFind = gCopyCommandLists.find(threadID) != gCopyCommandLists.end();
			if (isFind)
			{
				pCmdLists = &gCopyCommandLists[threadID];
			}
		}

		if (isFind == false)
		{
			std::lock_guard<std::shared_mutex> lock(gCopyCommandListMutex);
			gCopyCommandLists[threadID].resize(GetFrameBufferCount(), nullptr);
			pCmdLists = &gCopyCommandLists[threadID];
		}

		if (pCmdLists != nullptr)
		{
			if ((*pCmdLists)[bufferIndex] == nullptr)
			{
				std::lock_guard<std::shared_mutex> lock(gCopyCommandListMutex);
				(*pCmdLists)[bufferIndex] = static_cast<CopyCommandList*>(gCopyCommandQueue->RequestCommandList(priority));
			}

			result = (*pCmdLists)[bufferIndex];
			return result;
		}
		return nullptr;
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
		// DescriptiorAllcator 생성
		JG_CORE_INFO("Create DescriptorAllocator...");
		gCSUAllocator = CreateUniquePtr<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		gRTVAllocator = CreateUniquePtr<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		gDSVAllocator = CreateUniquePtr<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);


		JG_CORE_INFO("Create CommandQueue...");
		gGraphicsCommandQueue = CreateUniquePtr<CommandQueue>(gFrameBufferCount, D3D12_COMMAND_LIST_TYPE_DIRECT);
		gComputeCommandQueue  = CreateUniquePtr<CommandQueue>(gFrameBufferCount, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		gCopyCommandQueue     = CreateUniquePtr<CommandQueue>(gFrameBufferCount, D3D12_COMMAND_LIST_TYPE_COPY);


		JG_CORE_INFO("DirectX12 Init End");
		return true;
	}


	void DirectX12API::Destroy()
	{
		Flush();

		RootSignature::ClearCache();
		ResourceStateTracker::ClearCache();
		gFrameBuffers.clear();
		gGraphicsCommandLists.clear();
		gComputeCommandLists.clear();
		gCopyCommandLists.clear();

		
		gCSUAllocator.reset();
		gRTVAllocator.reset();
		gDSVAllocator.reset();

		gGraphicsCommandQueue.reset();
		gComputeCommandQueue.reset();
		gCopyCommandQueue.reset();


		gDevice.Reset();
		gFactory.Reset();
	}


	void DirectX12API::Begin()
	{
		gGraphicsCommandQueue->Begin();
		gComputeCommandQueue->Begin();
		gCopyCommandQueue->Begin();

		for (auto& cmdLists : gGraphicsCommandLists)
		{
			for (auto& cmdList : cmdLists.second)
			{
				cmdList = nullptr;
			}
		}
		for (auto& cmdLists : gComputeCommandLists)
		{
			for (auto& cmdList : cmdLists.second)
			{
				cmdList = nullptr;
			}
		}
		for (auto& cmdLists : gCopyCommandLists)
		{
			for (auto& cmdList : cmdLists.second)
			{
				cmdList = nullptr;
			}
		}
	}
	void DirectX12API::End()
	{
		// TODO
		// FrameBuffer Update
		for (auto& iter : gFrameBuffers)
		{
			iter.second->Update();
		}

		gGraphicsCommandQueue->End();
		gComputeCommandQueue->End();
		gCopyCommandQueue->End();


		// TODO
		// SwapBuffer
		for (auto& iter : gFrameBuffers)
		{
			iter.second->Present();
		}


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

	void DirectX12API::ClearRenderTarget(const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture)
	{
		auto commandList = GetGraphicsCommandList();

		for (auto& texture : rtTextures)
		{
			if (texture && texture->IsValid() == false) continue;
			auto handle = static_cast<DirectX12Texture*>(texture.get())->GetRTV();
			if (handle.ptr == 0) continue;


			auto info = texture->GetTextureInfo();

			commandList->ClearRenderTargetTexture(static_cast<DirectX12Texture*>(texture.get())->Get(), handle, info.ClearColor);
		}
		if (depthTexture && depthTexture->IsValid())
		{
			auto handle = static_cast<DirectX12Texture*>(depthTexture.get())->GetDSV();

			if (handle.ptr != 0)
			{
				auto info = depthTexture->GetTextureInfo();

				commandList->ClearDepthTexture(static_cast<DirectX12Texture*>(depthTexture.get())->Get(),
					handle, info.ClearDepth, info.ClearStencil);
			}
		}
	}

	void DirectX12API::SetRenderTarget(const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture)
	{
		auto commandList = GetGraphicsCommandList();

		List<ID3D12Resource*> d3dRTResources;
		List<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;

		ID3D12Resource* d3dDSResource = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = { 0 };

		for (auto& texture : rtTextures)
		{
			if (texture && texture->IsValid() == false) continue;
			auto handle = static_cast<DirectX12Texture*>(texture.get())->GetRTV();
			if (handle.ptr == 0) continue;


			d3dRTResources.push_back(static_cast<DirectX12Texture*>(texture.get())->Get());
			rtvHandles.push_back(handle);


		}
		if (depthTexture->IsValid())
		{
			auto handle = static_cast<DirectX12Texture*>(depthTexture.get())->GetDSV();
			if (handle.ptr != 0)
			{
				d3dDSResource = static_cast<DirectX12Texture*>(depthTexture.get())->Get();
			}
		}



		commandList->SetRenderTarget(d3dRTResources.data(), rtvHandles.data(), d3dRTResources.size(), d3dDSResource, &dsvHandle);
	}

	SharedPtr<IFrameBuffer> DirectX12API::CreateFrameBuffer(const FrameBufferInfo& info)
	{
		if (info.Handle == 0) return nullptr;
		auto iter = gFrameBuffers.find(info.Handle);
		if (iter != gFrameBuffers.end())
		{
			return gFrameBuffers[info.Handle];
		}

		auto buffer = CreateSharedPtr<DirectX12FrameBuffer>();
		if (!buffer->Init(info))
		{
			JG_CORE_ERROR("Failed Create DirectX12RenderContext");
			return nullptr;
		}
		gFrameBuffers.emplace(info.Handle, buffer);
		return buffer;
	}
	SharedPtr<IVertexBuffer> DirectX12API::CreateVertexBuffer(const String& name, void* datas, u64 elementSize, u64 elementCount)
	{
		auto vBuffer = CreateSharedPtr<DirectX12VertexBuffer>();
		vBuffer->SetName(name);
		if (!vBuffer->CreateBuffer(datas, elementSize, elementCount))
		{
			JG_CORE_ERROR("Failed Create VertexBuffer {0}", ws2s(name));
			return nullptr;
		}
		return vBuffer;
	}
	SharedPtr<IIndexBuffer> DirectX12API::CreateIndexBuffer(const String& name, u32* datas, u64 count)
	{
		auto iBuffer = CreateSharedPtr<DirectX12IndexBuffer>();
		iBuffer->SetName(name);
		if (!iBuffer->CreateBuffer(datas, count))
		{
			JG_CORE_ERROR("Failed Create IndexBuffer {0}", ws2s(name));
			return nullptr;
		}
		return iBuffer;
	}
	SharedPtr<IShader> DirectX12API::CreateShader(const String& sourceCode, EShaderFlags flags, const String& error)
	{
		auto shader = CreateSharedPtr<DirectX12Shader>();
		if (!shader->Compile(sourceCode, flags, error))
		{
			JG_CORE_ERROR("Failed Compile Shader \n Error : {0}  \n SourceCode : \n {1} ", ws2s(error), ws2s(sourceCode));
			return nullptr;
		}
		return shader;
	}
	SharedPtr<ITexture> DirectX12API::CreateTexture(const String& name, const TextureInfo& info)
	{

		auto texture = CreateSharedPtr<DirectX12Texture>();
		texture->Create(name, info);

		return texture;
	}
	SharedPtr<ITexture> DirectX12API::CreateTextureFromFile(const String& path)
	{
		return nullptr;
	}
	DXGI_FORMAT ConvertDirectX12TextureFormat(ETextureFormat format)
	{

		switch (format)
		{
		case ETextureFormat::R8G8B8A8_Unorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
		default:
			JG_CORE_ERROR("This {0} DirectX12 TextureFormat is not supported convert ETextureFormat", ws2s(TextureFormatToString(format)));
			return DXGI_FORMAT_UNKNOWN;
		}
	}
}
