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

	CommandList* CommandQueue::RequestCommandList(int priority) 
	{
		auto cmdList = RequestCommandList();
		mExpectExcuteCmdLists[priority].push_back(cmdList);
		return cmdList;
	}

	void CommandQueue::Begin()
	{
		uint64_t value = mFenceValue[mBufferIndex];
		mFence->WaitForFenceValue(value);

		
	}

	void CommandQueue::End()
	{
		// TODO
		// CommandList 완료되면 ㄱㄱ
	}

	void CommandQueue::Flush()
	{
		mFence->IncreaseValue();
		mD3DCmdQueue->Signal(mFence->Get(), mFence->GetValue());
		mFence->WaitForFenceValue(mFence->GetValue());
	}

	CommandList* CommandQueue::RequestCommandList()
	{
		auto& pendingQueue = mPendingCmdLists[mBufferIndex];
		UniquePtr<CommandList> cmdList = nullptr;
		CommandList* result = nullptr;
		if (!pendingQueue.empty())
		{
			cmdList = std::move(pendingQueue.front()); pendingQueue.pop();
			cmdList->Reset();
			result = cmdList.get();
			mCmdLists[cmdList.get()] = std::move(cmdList);
		}
		else
		{
			cmdList = CreateUniquePtr<CommandList>(mD3DType);
			result = cmdList.get();
			mCmdLists[cmdList.get()] = std::move(cmdList);
		}
		return result;
	}
}
