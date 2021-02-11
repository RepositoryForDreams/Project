#include "pch.h"
#include "CommandList.h"

#include "DescriptorAllocator.h"
#include "UploadAllocator.h"
#include "ResourceStateTracker.h"
#include "DynamicDescriptorAllocator.h"
#include "RootSignature.h"
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
		
		mD3DCommandList->RSSetViewports(
			1,
			&CD3DX12_VIEWPORT(viewport.TopLeftX, viewport.TopLeftY, viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth));
		
	}

	void GraphicsCommandList::SetViewports(const std::vector<Viewport>& viewports)
	{
		std::vector<D3D12_VIEWPORT> d3dViewports(viewports.size());
		for(i32 i = 0; i<(i32)d3dViewports.size(); ++i)
		{
			d3dViewports[i] = CD3DX12_VIEWPORT(
				viewports[i].TopLeftX, viewports[i].TopLeftY,
				viewports[i].Width, viewports[i].Height,
				viewports[i].MinDepth, viewports[i].MaxDepth);
		}
		mD3DCommandList->RSSetViewports(d3dViewports.size(), d3dViewports.data());
	}

	void GraphicsCommandList::SetScissorRect(const ScissorRect& rect)
	{
		mD3DCommandList->RSSetScissorRects(1, &CD3DX12_RECT(rect.Left, rect.Top, rect.Right, rect.Bottom));
	}

	void GraphicsCommandList::SetScissorRects(const std::vector<ScissorRect>& rects)
	{
		std::vector<D3D12_RECT> d3dRects(rects.size());
		for (i32 i = 0; i < (i32)d3dRects.size(); ++i)
		{
			d3dRects[i] = CD3DX12_RECT(rects[i].Left, rects[i].Top, rects[i].Right, rects[i].Bottom);
		}
		mD3DCommandList->RSSetScissorRects(d3dRects.size(), d3dRects.data());
	}

	void GraphicsCommandList::BindRootSignature(RootSignature& rootSig)
	{
		if(mBindedGraphicsRootSig.Get() == rootSig.Get())
		{
			return;
		}
		mBindedGraphicsRootSig = rootSig.Get();
		mD3DCommandList->SetGraphicsRootSignature(mBindedGraphicsRootSig.Get());
		mDynamicDescriptorAllocator->CommitRootSignature(rootSig);
	}

	void GraphicsCommandList::BindTexture(u32 rootParam, ID3D12Resource* texture, void* desc)
	{

		
	}

	void GraphicsCommandList::BindTextures(u32 rootParam, ID3D12Resource** textures, void** desc, uint32_t textureCount)
	{
	}

	void GraphicsCommandList::BindDynamicConstantBuffer(u32 rootParam, const void* data, u64 elementSize)
	{

		i32 initType = mDynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);
		auto alloc = mUploadAllocator->Allocate(elementSize, 256);

		memcpy(alloc.CPU, data, elementSize);


		switch(initType)
		{
		case RootSignature::__DescriptorTable__:
			{
				D3D12_DESCRIPTOR_RANGE_TYPE tableType = mDynamicDescriptorAllocator->GetDescriptorTableType(rootParam);
				switch (tableType)
				{
				case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
				{
					D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
					cbvDesc.BufferLocation = alloc.GPU;
					cbvDesc.SizeInBytes = (elementSize + 255) & ~255;
					auto cbvHandle = DirectX12API::CSUAllocate();
					DirectX12API::GetD3DDevice()->CreateConstantBufferView(&cbvDesc, cbvHandle.CPU());
					mDynamicDescriptorAllocator->CommitDescriptorTable(rootParam, { cbvHandle.CPU() });
				}
				break;
				default:
					JGASSERT("trying bind SRV or UAV or Sampler in BindConstantBuffer");
					break;
				}
			}
			break;
		case RootSignature::__ConstantBufferView__:
			mD3DCommandList->SetGraphicsRootConstantBufferView(rootParam, alloc.GPU);
			break;
		default:
			JGASSERT("BindDynamicConstantBuffer not support ShaderResourceView / UnorderedAccessView / Constant");
			break;
		}
	}

	void GraphicsCommandList::BindDynamicStructuredBuffer(u32 rootParam, const void* data, u64 elementSize, u64 elementCount)
	{
		
		i32  initType = mDynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);

		u64  btSize = elementSize * elementCount;
		auto alloc = mUploadAllocator->Allocate(btSize, elementSize);

		memcpy(alloc.CPU, data, btSize);
		switch (initType)
		{
		case RootSignature::__ShaderResourceView__:
			mD3DCommandList->SetGraphicsRootShaderResourceView(rootParam, alloc.GPU);
			break;
		case RootSignature::__UnorderedAccessView__:
			mD3DCommandList->SetGraphicsRootUnorderedAccessView(rootParam, alloc.GPU);
			break;
		default:
			assert("BindDynamicStructuredBuffer not support ConstantBufferView / Constant / DescriptorTable");
			break;
		}
	}

	void GraphicsCommandList::BindConstants(u32 rootParam, u32 btSize, void* data, u32 offset)
	{
		int initType = mDynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);
		switch (initType)
		{
		case RootSignature::__Constant__:
			mD3DCommandList->SetGraphicsRoot32BitConstants(rootParam, btSize / 4, data, offset / 4);
			break;
		default:
			assert("BindConstants not support CBV / SRV / UAV /DescriptorTable");
			break;
		}
	}

	void GraphicsCommandList::BindDynamicVertexBuffer(void* data, u64 elementSize, u64 elementCount)
	{
		u64 btSize = elementSize * elementCount;
		auto alloc = mUploadAllocator->Allocate(btSize, elementSize);

		memcpy(alloc.CPU, data, btSize);

		D3D12_VERTEX_BUFFER_VIEW bufferView;
		bufferView.BufferLocation = alloc.GPU;
		bufferView.SizeInBytes   = (u32)btSize;
		bufferView.StrideInBytes = (u32)elementSize;

		mD3DCommandList->IASetVertexBuffers(0, 1, &bufferView);
	}

	void GraphicsCommandList::BindDynamicIndexBuffer(const std::vector<u32>& indices)
	{
		u64 btSize = indices.size() * sizeof(u32);

		auto alloc = mUploadAllocator->Allocate(btSize, sizeof(u32));
		memcpy(alloc.CPU, indices.data(), btSize);

		D3D12_INDEX_BUFFER_VIEW bufferView;
		bufferView.BufferLocation = alloc.GPU;
		bufferView.Format = DXGI_FORMAT_R32_UINT;
		bufferView.SizeInBytes = (u32)indices.size();

		mD3DCommandList->IASetIndexBuffer(&bufferView);
		
	}
	void GraphicsCommandList::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology)
	{
		mD3DCommandList->IASetPrimitiveTopology(topology);
	}

}

