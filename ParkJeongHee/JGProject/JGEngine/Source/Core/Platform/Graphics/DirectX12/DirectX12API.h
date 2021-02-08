#pragma once
#include "Graphics/GraphicsAPI.h"
#include "Graphics/GraphicsDefine.h"


enum   DXGI_FORMAT;
struct ID3D12Device;
struct IDXGIFactory4;
struct ID3D12Resource;

namespace JG
{
	class IRenderContext;
	class  CommandQueue;
	class DescriptorAllocation;
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
	
	
	*/
	
	class DirectX12API : public IGraphicsAPI
	{
	public:
		virtual EGraphicsAPI GetAPI()const override;
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
	protected:
		// Application
		virtual bool Create() override;
		virtual void Destroy() override;
		// Renderer
		virtual void Begin() override;
		virtual void End()   override;
		virtual void Flush() override;
		virtual void SubmitRenderContext(SharedPtr<IRenderContext> renderContext) override;
	};


	DXGI_FORMAT ConvertDirectX12TextureFormat(ETextureFormat format);
}
