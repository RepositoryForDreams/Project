#include "pch.h"
#include "CommandList.h"
#include "UploadAllocator.h"
#include "ResourceStateTracker.h"
#include "DynamicDescriptorAllocator.h"
#include "../DirectX12API.h"


namespace JG
{
	CommandList::CommandList(D3D12_COMMAND_LIST_TYPE d3dType) : mD3DType(d3dType)
	{
		mD3DAllocator   = CreateD3DCommandAllocator(DirectX12API::GetD3DDevice(), mD3DType);
		mD3DCommandList = CreateD3DCommandList(DirectX12API::GetD3DDevice(), mD3DAllocator.Get(), mD3DType);

		mUploadAllocator			= CreateUniquePtr<UploadAllocator>();
		mResourceStateTracker	    = CreateUniquePtr<ResourceStateTracker>();
		mDynamicDescriptorAllocator = CreateUniquePtr<DynamicDescriptorAllocator>();
	}

	CommandList::~CommandList() = default;


	void CommandList::BackupResource(ID3D12Object* d3dObj)
	{
		mTempObjectList.push_back(d3dObj);
	}

	void CommandList::Reset()
	{

		mUploadAllocator->Reset();
		mResourceStateTracker->Reset();
		mDynamicDescriptorAllocator->Reset();
	

		mD3DAllocator->Reset();
		mD3DCommandList->Reset(mD3DAllocator.Get(), nullptr);
		mTempObjectList.clear();
		mBindedDescriptorHeap  = nullptr;
		mBindedGraphicsPSO     = nullptr;
		mBindedComputePSO	   = nullptr;
		mBindedGraphicsRootSig = nullptr;
		mBindedComputeRootSig  = nullptr;
	}

	void CommandList::Close()
	{
		mResourceStateTracker->FlushResourceBarrier(Get());
		mD3DCommandList->Close();
	}

	bool CommandList::Close(CommandList* commandList)
	{
		Close();
		bool isExistBarrier = mResourceStateTracker->FlushPendingResourceBarrier(commandList->Get());
		mResourceStateTracker->CommitResourceState();
		return false;
	}

	void CommandList::TransitionBarrier(ID3D12Resource* d3dResource, D3D12_RESOURCE_STATES state, u32 subResource, bool flush)
	{
		if (d3dResource == nullptr)
		{
			return;
		}
		mResourceStateTracker->TransitionBarrier(d3dResource, state, subResource);
		if (flush)
		{
			FlushResourceBarrier();
		}
		BackupResource(d3dResource);
	}

	void CommandList::UAVBarrier(ID3D12Resource* d3dResource, bool flush)
	{
		mResourceStateTracker->UAVBarrier(d3dResource);
		if (flush)
		{
			FlushResourceBarrier();
		}
		BackupResource(d3dResource);
	}

	void CommandList::AliasBarrier(ID3D12Resource* beforeResource, ID3D12Resource* afterResource, bool flush)
	{
		mResourceStateTracker->AliasBarrier(beforeResource, afterResource);
		if (flush)
		{
			FlushResourceBarrier();
		}
		BackupResource(beforeResource);
		BackupResource(afterResource);
	}

	void CommandList::FlushResourceBarrier()
	{
		mResourceStateTracker->FlushResourceBarrier(Get());
	}

	void CommandList::CopyResource(ID3D12Resource* dest, ID3D12Resource* src)
	{
		if (dest == nullptr || src == nullptr)
		{
			return;
		}
		TransitionBarrier(dest, D3D12_RESOURCE_STATE_COPY_DEST);
		TransitionBarrier(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
		FlushResourceBarrier();



		mD3DCommandList->CopyResource(dest, src);


		BackupResource(dest);
		BackupResource(src);

	}

	void GraphicsCommandList::SetViewport(const Viewport& viewport)
	{
	}

	void GraphicsCommandList::SetViewports(const std::vector<Viewport>& viewports)
	{
	}

	void GraphicsCommandList::SetScissorRect(const ScissorRect& rect)
	{
	}

	void GraphicsCommandList::SetScissorRects(const std::vector<ScissorRect>& rects)
	{
	}

	void GraphicsCommandList::BindRootSignature(RootSignature& rootSig)
	{
	}

	void GraphicsCommandList::BindTexture(u32 rootParam, ID3D12Resource* texture, void* desc)
	{
	}

	void GraphicsCommandList::BindTextures(u32 rootParam, ID3D12Resource** textures, void** desc, uint32_t textureCount)
	{
	}

	void GraphicsCommandList::BindDynamicConstantBuffer(u32 rootparam, const void* data, u64 elementSize)
	{
	}

	void GraphicsCommandList::BindDynamicStructuredBuffer(u32 rootparam, const void* data, u64 elementSize, u64 elementCount)
	{
	}

	void GraphicsCommandList::BindConstants(u32 rootparam, u32 btSize, void* data, u32 offset)
	{
	}

	void GraphicsCommandList::BindDynamicVertexBuffer(void* data, u64 elementSize, u64 elementCount)
	{
	}

	void GraphicsCommandList::BindDynamicIndexBuffer(const std::vector<u32>& indices)
	{
	}

	void GraphicsCommandList::BindDynamicIndexBuffer(const u32* iData, u64 count)
	{
	}

	void GraphicsCommandList::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology)
	{
	}

}

