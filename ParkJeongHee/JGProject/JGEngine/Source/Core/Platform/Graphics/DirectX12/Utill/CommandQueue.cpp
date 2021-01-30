#include "pch.h"
#include "CommandQueue.h"
#include "Fence.h"
#include "CommandList.h"

#include "../DirectX12API.h"


namespace JG
{
	CommandQueue::CommandQueue(u64 bufferCount, D3D12_COMMAND_LIST_TYPE type)
	{
		mD3DType = type;
		mFence = CreateUniquePtr<Fence>();
		mD3DCmdQueue = CreateD3DCommandQueue(DirectX12API::GetD3DDevice(), mD3DType);
		mFenceValue.resize(bufferCount);
	}

	CommandList* CommandQueue::RequestCommandList(int priority) const
	{
		return nullptr;
	}

	void CommandQueue::Begin()
	{
		
	}

	void CommandQueue::End()
	{
	}

	void CommandQueue::Flush()
	{
	}

	CommandList* CommandQueue::RequestCommandList() const
	{
		return nullptr;
	}
}
