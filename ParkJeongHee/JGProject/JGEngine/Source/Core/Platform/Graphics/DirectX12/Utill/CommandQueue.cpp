#include "pch.h"
#include "CommandQueue.h"
#include "CommandList.h"
#include "Fence.h"
#include "ResourceStateTracker.h"
#include "../DirectX12API.h"


namespace JG
{
	CommandQueue::CommandQueue(u64 bufferCount, D3D12_COMMAND_LIST_TYPE type)
	{
		mD3DType = type;
		mFence = CreateUniquePtr<Fence>();
		mD3DCmdQueue = CreateD3DCommandQueue(DirectX12API::GetD3DDevice(), mD3DType);
		mFenceValue.resize(bufferCount, 0);
	}

	CommandQueue::~CommandQueue() = default;

	CommandList* CommandQueue::RequestCommandList(i32 priority) 
	{
		auto cmdList = RequestCommandList();
		mExpectExcuteCmdLists[priority].push_back(cmdList);
		return cmdList;
	}

	void CommandQueue::Begin()
	{
		uint64_t value = mFenceValue[DirectX12API::GetFrameBufferIndex()];
		mFence->WaitForFenceValue(value);

		
	}

	void CommandQueue::End()
	{
		std::vector<ID3D12CommandList*>   d3dCmdLists;
		ResourceStateTracker::Lock();
		for (auto& cmdLists : mExpectExcuteCmdLists)
		{

			for (auto& cmdList : cmdLists.second)
			{
				auto pendingCmdList = RequestCommandList();

				bool has_pending_barrier = cmdList->Close(pendingCmdList);

				pendingCmdList->Close();
				if (has_pending_barrier)
				{
					d3dCmdLists.push_back(pendingCmdList->Get());
				}
				d3dCmdLists.push_back(cmdList->Get());

				mPendingCmdLists[DirectX12API::GetFrameBufferIndex()].push(move(mCmdLists[pendingCmdList]));
				mPendingCmdLists[DirectX12API::GetFrameBufferIndex()].push(move(mCmdLists[cmdList]));
				mCmdLists.erase(pendingCmdList);
				mCmdLists.erase(cmdList);
			}
		}
		ResourceStateTracker::UnLock();
		mExpectExcuteCmdLists.clear();
		mD3DCmdQueue->ExecuteCommandLists((uint32_t)d3dCmdLists.size(), d3dCmdLists.data());



		mFence->IncreaseValue();
		mD3DCmdQueue->Signal(mFence->Get(), mFence->GetValue());
		mFenceValue[DirectX12API::GetFrameBufferIndex()] = mFence->GetValue();
	}

	void CommandQueue::Flush()
	{
		mFence->IncreaseValue();
		mD3DCmdQueue->Signal(mFence->Get(), mFence->GetValue());
		mFence->WaitForFenceValue(mFence->GetValue());
	}

	CommandList* CommandQueue::RequestCommandList()
	{
		auto& pendingQueue = mPendingCmdLists[DirectX12API::GetFrameBufferIndex()];
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
