#pragma once
#include "Graphics/GraphicsAPI.h"




struct ID3D12Device;
struct IDXGIFactory4;
struct ID3D12Resource;
namespace JG
{
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
		static u64			  GetFrameBufferIndex();
	protected:
		// Application
		virtual bool Create() override;
		virtual void Destroy() override;
		// Renderer
		virtual void Begin() override;
		virtual void End()   override;
		virtual void Flush() override;

		
	};
}
