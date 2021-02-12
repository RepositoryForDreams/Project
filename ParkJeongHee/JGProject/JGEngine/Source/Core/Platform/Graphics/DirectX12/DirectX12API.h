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
	// �� ó��
	// CommandList ���� �� Reset
	// CommandList SetViewport, ���� Rect ����
	// ClearRenderTarget
	// SetRenderTarget
	// SetDescriptionHeaps (�ؽ��� �Ҵ��� DescriptionHeap)
	//
	// 
	// �߰�
	// ������ ���ε�
	// Vertex, Index Buffer ���ε�
	// DrawCall
	//
	//
	// ��
	// Close
	// CommandQueue �� ������



	/* 
	* ���� ��
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
	protected:
		// Application
		virtual bool Create() override;
		virtual void Destroy() override;
		// API
		virtual void Begin() override;
		virtual void End()   override;
		virtual void Flush() override;
		virtual void SubmitRenderContext(SharedPtr<IRenderContext> renderContext) override;

		// Renderer2D
		virtual void Renderer2D_Begin_Impl() override;
		virtual void Renderer2D_End_Impl()   override;
	protected:
		virtual SharedPtr<IRenderContext> CreateRenderContext(const RenderContextSettings& settings) override;
		virtual SharedPtr<IVertexBuffer>  CreateVertexBuffer(String name, void* datas, u64 elementSize, u64 elementCount) override;
		virtual SharedPtr<IIndexBuffer>   CreateIndexBuffer(String name, u32* datas, u32 count) override;
		virtual SharedPtr<IShader>        CreateShader(const String& sourceCode, ShaderFlags flags, const String& error) override;
	private:
		bool ReadyRenderer2D();
	};


	DXGI_FORMAT ConvertDirectX12TextureFormat(ETextureFormat format);
}
