#pragma once
#include "Graphics/GraphicsAPI.h"
#include "Graphics/GraphicsDefine.h"


enum   DXGI_FORMAT;
struct ID3D12Device;
struct IDXGIFactory4;
struct ID3D12Resource;

namespace JG
{
	class ITexture;
	class  IFrameBuffer;
	class RootSignature;
	class  CommandQueue;
	class  DescriptorAllocation;
	class GraphicsCommandList;
	class ComputeCommandList;
	class CopyCommandList;
	class GraphicsPipelineState;
	class ComputePipelineState;
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

		static GraphicsCommandList* GetGraphicsCommandList();
		static ComputeCommandList*  GetComputeCommandList();
		static CopyCommandList*     GetCopyCommandList();

		static SharedPtr<GraphicsPipelineState> GetGraphicsPipelineState();
		static SharedPtr<ComputePipelineState>  GetComputePipelineState();
		static SharedPtr<RootSignature>			GetRootSignature();
	protected:
		// Application
		virtual bool Create() override;
		virtual void Destroy() override;
		// API
		virtual void Begin() override;
		virtual void End()   override;
		virtual void Flush() override;

	protected:
		virtual void SetViewports(const List<Viewport>& viewPorts) override;
		virtual void SetScissorRects(const List<ScissorRect>& scissorRects) override;
		virtual void ClearRenderTarget(const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture) override;
		virtual void SetRenderTarget(const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture) override;
		virtual void DrawIndexed(u32 indexCount, u32 instancedCount = 1, u32 startIndexLocation = 0, u32 startVertexLocation = 0, u32 startInstanceLocation = 0) override;
	protected:
		virtual void SetDepthStencilState(EDepthStencilStateTemplate _template) override;
		virtual void SetBlendState(u32 renderTargetSlot, EBlendStateTemplate _template) override;
		virtual void SetRasterizerState(ERasterizerStateTemplate _template) override;
	protected:
		virtual SharedPtr<IFrameBuffer>   CreateFrameBuffer(const FrameBufferInfo& info) override;
		virtual SharedPtr<IVertexBuffer>  CreateVertexBuffer(const String& name, EBufferLoadMethod method) override;
		virtual SharedPtr<IIndexBuffer>   CreateIndexBuffer(const String& name, EBufferLoadMethod method) override;
		virtual SharedPtr<IShader>        CreateShader(const String& name, const String& sourceCode, EShaderFlags flags) override;
		virtual SharedPtr<IMaterial>	  CreateMaterial(const String& name, SharedPtr<IShader> shader) override;
		virtual SharedPtr<IMesh>		  CreateMesh(const String& name) override;
		virtual SharedPtr<ITexture>       CreateTexture(const String& name, const TextureInfo& info) override;
		virtual SharedPtr<ITexture>       CreateTextureFromFile(const String& path) override;
	};
}
 