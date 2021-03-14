﻿#include "pch.h"
#include "CommandList.h"

#include "DescriptorAllocator.h"
#include "UploadAllocator.h"
#include "ResourceStateTracker.h"
#include "DynamicDescriptorAllocator.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"


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
		mTempObjectList.clear();

		mD3DAllocator->Reset();
		mD3DCommandList->Reset(mD3DAllocator.Get(), nullptr);
	
		mBindedDescriptorHeap  = nullptr;
		mBindedPipelineState   = nullptr;
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
		return isExistBarrier;
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

	void CommandList::CopyBuffer(ID3D12Resource* buffer, void* data, u64 elementSize, u64 elementCount)
	{
		if (buffer == nullptr)
		{
			return;
		}
		auto d3dDevice = DirectX12API::GetD3DDevice();
		u64 btSize = elementSize * elementCount;

		ComPtr<ID3D12Resource> uploadBuffer;
		HRESULT hResult = d3dDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(btSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.GetAddressOf()));
		uploadBuffer->SetName(TT("UploadBuffer"));

		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData      = data;
		subResourceData.RowPitch   = (uint32_t)btSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		TransitionBarrier(buffer, D3D12_RESOURCE_STATE_COPY_DEST);
		UpdateSubresources(mD3DCommandList.Get(), buffer, uploadBuffer.Get(), 0, 0, 1, &subResourceData);

		BackupResource(buffer);
		BackupResource(uploadBuffer.Get());
	}



	void GraphicsCommandList::Reset()
	{
		CommandList::Reset();
		mVertexViews.clear();
	}

	void GraphicsCommandList::SetViewport(const Viewport& viewport)
	{
		
		mD3DCommandList->RSSetViewports(
			1,
			&CD3DX12_VIEWPORT(viewport.TopLeftX, viewport.TopLeftY, viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth));
		
	}

	void GraphicsCommandList::SetViewports(const List<Viewport>& viewports)
	{
		List<D3D12_VIEWPORT> d3dViewports(viewports.size());
		for(i32 i = 0; i<(i32)d3dViewports.size(); ++i)
		{
			d3dViewports[i] = CD3DX12_VIEWPORT(
				viewports[i].TopLeftX, viewports[i].TopLeftY,
				viewports[i].Width, viewports[i].Height,
				viewports[i].MinDepth, viewports[i].MaxDepth);
		}
		mD3DCommandList->RSSetViewports((u32)d3dViewports.size(), d3dViewports.data());
	}

	void GraphicsCommandList::SetScissorRect(const ScissorRect& rect)
	{
		mD3DCommandList->RSSetScissorRects(1, &CD3DX12_RECT(rect.Left, rect.Top, rect.Right, rect.Bottom));
	}

	void GraphicsCommandList::SetScissorRects(const List<ScissorRect>& rects)
	{
		List<D3D12_RECT> d3dRects(rects.size());
		for (i32 i = 0; i < (i32)d3dRects.size(); ++i)
		{
			d3dRects[i] = CD3DX12_RECT(rects[i].Left, rects[i].Top, rects[i].Right, rects[i].Bottom);
		}
		mD3DCommandList->RSSetScissorRects((u32)d3dRects.size(), d3dRects.data());
	}

	void GraphicsCommandList::ClearRenderTargetTexture(ID3D12Resource* resource, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const Color& clearColor)
	{
		if (resource == nullptr || rtvHandle.ptr == 0) return;

		TransitionBarrier(resource, D3D12_RESOURCE_STATE_RENDER_TARGET);
		FlushResourceBarrier();


		FLOAT color[4] = { clearColor.R, clearColor.G, clearColor.B, clearColor.A };
		mD3DCommandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);

	}

	void GraphicsCommandList::ClearDepthTexture(ID3D12Resource* resource, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, f32 clearDepth, u8 clearStencil, D3D12_CLEAR_FLAGS clearFlags)
	{
		if (resource == nullptr || dsvHandle.ptr == 0) return;


		TransitionBarrier(resource, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		FlushResourceBarrier();



		mD3DCommandList->ClearDepthStencilView(dsvHandle, clearFlags, clearDepth, clearStencil, 0, nullptr);
	}

	void GraphicsCommandList::SetRenderTarget(
		ID3D12Resource** rtTextures, D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandles, u64 rtTextureCount,
		ID3D12Resource* depthTexture, D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle)
	{
		for (u64 i = 0; i < rtTextureCount; ++i)
		{
			if (rtTextures[i] == nullptr) continue;
			TransitionBarrier(rtTextures[i], D3D12_RESOURCE_STATE_RENDER_TARGET);
		}
		if (depthTexture != nullptr)
		{
			TransitionBarrier(depthTexture, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		}
		FlushResourceBarrier();


		mD3DCommandList->OMSetRenderTargets(rtTextureCount, rtvHandles, false, dsvHandle);
	}

	void GraphicsCommandList::BindRootSignature(SharedPtr<RootSignature> rootSig)
	{
		if(mBindedGraphicsRootSig.Get() == rootSig->Get())
		{
			return;
		}
		mBindedGraphicsRootSig = rootSig->Get();
		mD3DCommandList->SetGraphicsRootSignature(mBindedGraphicsRootSig.Get());
		mDynamicDescriptorAllocator->CommitRootSignature(*rootSig);
	}

	void GraphicsCommandList::BindPipelineState(SharedPtr<GraphicsPipelineState> pso)
	{
		if (mBindedPipelineState.Get() == pso->Get())
		{
			return;
		}
		mBindedPipelineState = pso->Get();
		mD3DCommandList->SetPipelineState(mBindedPipelineState.Get());
	}



	void GraphicsCommandList::BindTextures(u32 rootParam, List<D3D12_CPU_DESCRIPTOR_HANDLE> handles)
	{
		i32 initType = mDynamicDescriptorAllocator->GetDescriptorInitAsType(rootParam);


		switch (initType)
		{
		case RootSignature::__DescriptorTable__:
		{
			D3D12_DESCRIPTOR_RANGE_TYPE tableType = mDynamicDescriptorAllocator->GetDescriptorTableType(rootParam);
			switch (tableType)
			{
			case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
			case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
				mDynamicDescriptorAllocator->CommitDescriptorTable(rootParam, handles);
				break;
			default:
				JGASSERT("trying bind CBV or Sampler in BindTextures");
				break;
			}
		}
			break;
		case RootSignature::__ShaderResourceView__:
			break;
		case RootSignature::__UnorderedAccessView__:
			break;
		default:
			JGASSERT("BindTextures not support ConstantBufferView / Constant");
			break;
		}



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

	void GraphicsCommandList::BindVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& view, bool isFlush)
	{
		mVertexViews.push_back(view);
		if (isFlush == true)
		{
			FlushVertexBuffer();
		}

	}

	void GraphicsCommandList::BindDynamicVertexBuffer(void* data, u64 elementCount, u64 elementSize, bool isFlush)
	{
		u64 btSize = elementSize * elementCount;
		auto alloc = mUploadAllocator->Allocate(btSize, elementSize);

		memcpy(alloc.CPU, data, btSize);

		D3D12_VERTEX_BUFFER_VIEW bufferView;
		bufferView.BufferLocation = alloc.GPU;
		bufferView.SizeInBytes   = (u32)btSize;
		bufferView.StrideInBytes = (u32)elementSize;

		mVertexViews.push_back(bufferView);

		if (isFlush == true)
		{
			FlushVertexBuffer();
		}
	}

	void GraphicsCommandList::FlushVertexBuffer()
	{
		mD3DCommandList->IASetVertexBuffers(0, mVertexViews.size(), mVertexViews.data());
	}


	void GraphicsCommandList::BindIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& view)
	{
		mD3DCommandList->IASetIndexBuffer(&view);
	}

	void GraphicsCommandList::BindDynamicIndexBuffer(u32* datas, u64 count)
	{
		u64 btSize = count * sizeof(u32);
		
		auto alloc = mUploadAllocator->Allocate(btSize, sizeof(u32));
		memcpy(alloc.CPU, datas, btSize);

		D3D12_INDEX_BUFFER_VIEW bufferView;
		bufferView.BufferLocation = alloc.GPU;
		bufferView.Format = DXGI_FORMAT_R32_UINT;
		bufferView.SizeInBytes = (u32)btSize;

		mD3DCommandList->IASetIndexBuffer(&bufferView);
	}
	void GraphicsCommandList::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology)
	{
		mD3DCommandList->IASetPrimitiveTopology(topology);
	}

	void GraphicsCommandList::DrawIndexed(u32 indexCount, u32 instancedCount, u32 startIndexLocation,
		u32 startVertexLocation, u32 startInstanceLocation)
	{
		mDynamicDescriptorAllocator->PushDescriptorTable(mD3DCommandList, mBindedDescriptorHeap, true);
		mD3DCommandList->DrawIndexedInstanced(indexCount, instancedCount, startIndexLocation, startVertexLocation, startInstanceLocation);
	}
}

