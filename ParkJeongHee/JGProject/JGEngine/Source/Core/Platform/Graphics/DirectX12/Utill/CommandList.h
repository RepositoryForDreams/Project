#pragma once

#include "JGCore.h"
#include "Graphics/GraphicsDefine.h"
#include "DirectX12Helper.h"


namespace JG
{
	class ResourceStateTracker;
	class UploadAllocator;
	class DynamicDescriptorAllocator;
	class RootSignature;
	class CommandList
	{
	protected:
		ComPtr<ID3D12PipelineState> mBindedGraphicsPSO = nullptr;
		ComPtr<ID3D12PipelineState> mBindedComputePSO = nullptr;
		ComPtr<ID3D12RootSignature> mBindedGraphicsRootSig = nullptr;
		ComPtr<ID3D12RootSignature> mBindedComputeRootSig = nullptr;

		ComPtr<ID3D12DescriptorHeap> mBindedDescriptorHeap = nullptr;

		D3D12_COMMAND_LIST_TYPE           mD3DType;
		ComPtr<ID3D12GraphicsCommandList> mD3DCommandList;
		ComPtr<ID3D12CommandAllocator>    mD3DAllocator;

		std::vector<ComPtr<ID3D12Object>> mTempObjectList;

		UniquePtr<UploadAllocator>		mUploadAllocator;
		UniquePtr<ResourceStateTracker> mResourceStateTracker;
		UniquePtr<DynamicDescriptorAllocator> mDynamicDescriptorAllocator;
	public:
		CommandList(D3D12_COMMAND_LIST_TYPE d3dType);
		virtual ~CommandList();
	public:
		ID3D12GraphicsCommandList* Get() const {
			return mD3DCommandList.Get();
		}
		void BackupResource(ID3D12Object* d3dObj);
		void Reset();
		void Close();
		bool Close(CommandList* commandList);

		void TransitionBarrier(ID3D12Resource* d3dResource, D3D12_RESOURCE_STATES state, u32 subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flush = false);
		void UAVBarrier(ID3D12Resource* d3dResource, bool flush = false);
		void AliasBarrier(ID3D12Resource* beforeResource, ID3D12Resource* afterResource, bool flush = false);
		void FlushResourceBarrier();

		void CopyResource(ID3D12Resource* dest, ID3D12Resource* src);
		//void CopyBuffer(ID3D12Resource* buffer, void* data, u64 elementSize, u64 elementCount);
	};

	class GraphicsCommandList : public CommandList
	{
	public:
		virtual ~GraphicsCommandList() = default;
	public:
		void SetViewport(const Viewport& viewport);
		void SetViewports(const std::vector<Viewport>& viewports);
		void SetScissorRect(const ScissorRect& rect);
		void SetScissorRects(const std::vector<ScissorRect>& rects);
		//void ClearRenderTarget(GraphicsCommandKeyPtr cmdKey, RenderTarget& renderTarget, D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL);
		//void SetRenderTarget(GraphicsCommandKeyPtr cmdKey, RenderTarget& renderTarget);
		void BindRootSignature(RootSignature& rootSig);
		//void BindPipelineState(GraphicsCommandKeyPtr cmdKey, GraphicsPipelineState& pso);
		
		void BindTexture(u32 rootParam, ID3D12Resource* texture, void* desc = nullptr);
		void BindTextures(u32 rootParam, ID3D12Resource** textures, void** desc, uint32_t textureCount);


		//void BindConstantBuffer(uint32_t rootparam, ConstantBuffer& buffer);
		void BindDynamicConstantBuffer(u32 rootParam, const void* data, u64 elementSize);


		// void BindStructuredBuffer(u32 rootparam, StructuredBuffer& buffer);
		void BindDynamicStructuredBuffer(u32 rootParam, const  void* data, u64 elementSize, u64 elementCount);

		void BindConstants(u32 rootparam, u32 btSize, void* data, u32 offset = 0);

		// void BindVertexBuffer(VertexBuffer& vBuffer);
		void BindDynamicVertexBuffer(void* data, u64 elementSize, u64 elementCount);



		// void BindIndexBuffer(IndexBuffer& iBuffer);
		void BindDynamicIndexBuffer(const std::vector<u32>& indices);
		void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology);


		void DrawIndexed(u32 indexCount, u32 instancedCount = 1, u32 startIndexLocation = 0, u32 startVertexLocation = 0, u32 startInstanceLocation = 0);
	};

	
}

