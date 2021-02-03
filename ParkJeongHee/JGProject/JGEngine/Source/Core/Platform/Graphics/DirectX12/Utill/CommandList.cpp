#include "pch.h"
#include "CommandList.h"
#include "UploadAllocator.h"
#include "ResourceStateTracker.h"
#include "UploadAllocator.h"
#include "../DirectX12API.h"


namespace JG
{
	CommandList::CommandList(D3D12_COMMAND_LIST_TYPE d3dType) : mD3DType(d3dType)
	{
		mD3DAllocator   = CreateD3DCommandAllocator(DirectX12API::GetD3DDevice(), mD3DType);
		mD3DCommandList = CreateD3DCommandList(DirectX12API::GetD3DDevice(), mD3DAllocator.Get(), mD3DType);

		mUploadAllocator	  = CreateUniquePtr<UploadAllocator>();
		mResourceStateTracker = CreateUniquePtr<ResourceStateTracker>();
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

	

		mD3DAllocator->Reset();
		mD3DCommandList->Reset(mD3DAllocator.Get(), nullptr);
		mTempObjectList.clear();
		mBindedDescriptorHeap = nullptr;
		mBindedGraphicsPSO = nullptr;
		mBindedComputePSO = nullptr;
		mBindedGraphicsRootSig = nullptr;
		mBindedComputeRootSig = nullptr;
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

}

