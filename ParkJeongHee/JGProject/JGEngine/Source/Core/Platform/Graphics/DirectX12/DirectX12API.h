#pragma once
#include "Graphics/GraphicsAPI.h"
#include "Graphics/GraphicsDefine.h"


enum   DXGI_FORMAT;
struct ID3D12Device;
struct IDXGIFactory4;
struct ID3D12Resource;

namespace JG
{
	class  IRenderContext;
	class  CommandQueue;
	class  DescriptorAllocation;
	class GraphicsCommandList;
	class ComputeCommandList;
	class CopyCommandList;
	// Rendering Flow
	//
	// 맨 처음
	// CommandList 생성 및 Reset
	// CommandList SetViewport, 가위 Rect 설정
	// ClearRenderTarget
	// SetRenderTarget
	// SetDescriptionHeaps (텍스쳐 할당한 DescriptionHeap)
	//
	// 
	// 중간
	// 데이터 바인딩
	// Vertex, Index Buffer 바인딩
	// DrawCall
	//
	//
	// 끝
	// Close
	// CommandQueue 에 떤져줌



	/* 
	* 설정 값
	* BufferCount = 3
	* DynamicDescriptorAllocator Size
	* UploadAllocator Size 
	* Shader Version
	
	*/
	
	class DirectX12API : public IGraphicsAPI
	{
	public:
		virtual EGraphicsAPI GetAPI()const override;
		virtual void SubmitRenderContext(SharedPtr<IRenderContext> renderContext) override;
	public:
		static IDXGIFactory4* GetDXGIFactory();
		static ID3D12Device*  GetD3DDevice();
		static CommandQueue*  GetGraphicsCommandQueue();
		static CommandQueue*  GetComputeCommandQueue();
		static CommandQueue*  GetCopyCommandQueue();
		static u64			  GetFrameBufferCount();
		static u64			  GetFrameBufferIndex();


		static DescriptorAllocation RTVAllocate();
		static DescriptorAllocation DSVAllocate();
		static DescriptorAllocation CSUAllocate();

		static GraphicsCommandList* GetGraphicsCommandList(i32 priority = 0);
		static ComputeCommandList*  GetComputeCommandList(i32 priority = 0);
		static CopyCommandList*     GetCopyCommandList(i32 priority = 0);
	protected:
		// Application
		virtual bool Create() override;
		virtual void Destroy() override;
		// API
		virtual void Begin() override;
		virtual void End()   override;
		virtual void Flush() override;

	protected:
		virtual SharedPtr<IRenderContext> CreateRenderContext(const RenderContextSettings& settings) override;
		virtual SharedPtr<IVertexBuffer>  CreateVertexBuffer(const String& name, void* datas, u64 elementSize, u64 elementCount) override;
		virtual SharedPtr<IIndexBuffer>   CreateIndexBuffer(const String& name, u32* datas, u64 count) override;
		virtual SharedPtr<IShader>        CreateShader(const String& sourceCode, EShaderFlags flags, const String& error) override;
	};


	DXGI_FORMAT ConvertDirectX12TextureFormat(ETextureFormat format);
}
