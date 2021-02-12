#include "pch.h"
#include "DirectX12API.h"
#include "DirectX12FrameBuffer.h"
#include "DirectX12RenderContext.h"
#include "DirectX12Resource.h"
#include "DirectX12Shader.h"
#include "Utill/DirectX12Helper.h"
#include "Utill/DescriptorAllocator.h"
#include "Utill/CommandQueue.h"
#include "Utill/RootSignature.h"
#include "Utill/PipelineState.h"

namespace JG
{
	struct Renderer2DItem
	{
		u64 IdentificationID = 0; // PSO ID
		u64 TextureIndex     = 0; // 텍스쳐 ID
	};
	struct Renderer2DData
	{
		// 공용 데이터
		UniquePtr<DirectX12Texture> RTTexture;
		Color ClearColor;



		// DirectX12 에서만 필요한 데이터
		UniquePtr<RootSignature>          RootSignature;
		UniquePtr<GraphicsPipelineState>  TexturePSO;
		UniquePtr<GraphicsPipelineState>  TextPSO;
	};





	static ComPtr<IDXGIFactory4> gFactory;
	static ComPtr<ID3D12Device>  gDevice;
	static UniquePtr<DescriptorAllocator> gCSUAllocator;
	static UniquePtr<DescriptorAllocator> gRTVAllocator;
	static UniquePtr<DescriptorAllocator> gDSVAllocator;

	static UniquePtr<CommandQueue> gGraphicsCommandQueue;
	static UniquePtr<CommandQueue> gComputeCommandQueue;
	static UniquePtr<CommandQueue> gCopyCommandQueue;
	static UniquePtr<Renderer2DData> gRenderer2DData;

	static std::vector<UniquePtr<DirectX12FrameBuffer>> gFrameBuffers;
	static std::unordered_map<ptraddr, SharedPtr<IRenderContext>> gRenderContexts;
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

		JG_CORE_INFO("Create FrameBuffer...");
		for (u64 i = 0; i < gFrameBufferCount; ++i)
		{
			// FrameBuffer 생성
		}

		if (!ReadyRenderer2D())
		{
			return false;
		}
		// FrameBuffer 생성
		JG_CORE_INFO("DirectX12 Init End");
		return true;
	}


	void DirectX12API::Destroy()
	{
		Flush();

		RootSignature::ClearCache();
		gRenderContexts.clear();


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
		for (auto& iter : gRenderContexts)
		{
			iter.second->Update();
		}

		gGraphicsCommandQueue->End();
		gComputeCommandQueue->End();
		gCopyCommandQueue->End();


		// TODO
		// SwapBuffer
		for (auto& iter : gRenderContexts)
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
	void DirectX12API::SubmitRenderContext(SharedPtr<IRenderContext> renderContext)
	{
		if (renderContext == nullptr)
		{
			return;
		}
		auto settings = renderContext->GetSettings();
		auto iter = gRenderContexts.find(settings.Handle);
		if (iter != gRenderContexts.end())
		{
			return;
		}
		gRenderContexts.emplace(settings.Handle, renderContext);

	}
	void DirectX12API::Renderer2D_Begin_Impl()
	{


	}
	void DirectX12API::Renderer2D_End_Impl()
	{


	}
	SharedPtr<IRenderContext> DirectX12API::CreateRenderContext(const RenderContextSettings& settings)
	{
		auto context = CreateSharedPtr<DirectX12RenderContext>();
		if (!context->Init(settings))
		{
			JG_CORE_ERROR("Failed Create DirectX12RenderContext");
			return nullptr;
		}
		return context;
	}
	SharedPtr<IVertexBuffer> DirectX12API::CreateVertexBuffer(String name, void* datas, u64 elementSize, u64 elementCount)
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
	SharedPtr<IIndexBuffer> DirectX12API::CreateIndexBuffer(String name, u32* datas, u32 count)
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
	SharedPtr<IShader> DirectX12API::CreateShader(const String& sourceCode, ShaderFlags flags, const String& error)
	{
		auto shader = CreateSharedPtr<DirectX12Shader>();
		if (!shader->Compile(sourceCode, flags, error))
		{
			JG_CORE_ERROR("Failed Compile Shader \n Error : {0}  \n SourceCode : \n {1} ", ws2s(error), ws2s(sourceCode));
			return nullptr;
		}
		return shader;
	}
	bool DirectX12API::ReadyRenderer2D()
	{
		// NOTE
// Renderer2D 준비
		JG_CORE_INFO("Ready Renderer2D...");
		gRenderer2DData = CreateUniquePtr<Renderer2DData>();



		//auto CreateShader("")




		return false;
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





	// DirectX 12 Renderer2D API



}
