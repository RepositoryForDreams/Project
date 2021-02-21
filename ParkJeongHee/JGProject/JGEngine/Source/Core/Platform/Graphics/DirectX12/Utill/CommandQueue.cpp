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

		while (mPendingCmdLists[DirectX12API::GetFrameBufferIndex()].empty() == false)
		{
			mCurrentPendingCmdLists.push(std::move(mPendingCmdLists[DirectX12API::GetFrameBufferIndex()].front()));
			mPendingCmdLists[DirectX12API::GetFrameBufferIndex()].pop();
		}
	}

	void CommandQueue::End()
	{
		List<ID3D12CommandList*>   d3dCmdLists;
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
		UniquePtr<CommandList> cmdList = nullptr;
		CommandList* result = nullptr;
		if (!mCurrentPendingCmdLists.empty())
		{
			cmdList = std::move(mCurrentPendingCmdLists.front()); mCurrentPendingCmdLists.pop();
			cmdList->Reset();
			result = cmdList.get();
			mCmdLists[cmdList.get()] = std::move(cmdList);
		}
		else
		{
			switch (mD3DType)
			{
			case D3D12_COMMAND_LIST_TYPE_DIRECT:
				cmdList = CreateUniquePtr<GraphicsCommandList>(mD3DType);
				break;
			case D3D12_COMMAND_LIST_TYPE_COMPUTE:
				cmdList = CreateUniquePtr<ComputeCommandList>(mD3DType);
				break;
			default:
				cmdList = CreateUniquePtr<CommandList>(mD3DType);
				break;
			}
		
			result = cmdList.get();
			mCmdLists[cmdList.get()] = std::move(cmdList);
		}
		return result;
	}
}
