#pragma once



#include "JGCore.h"
#include "DirectX12Helper.h"



namespace JG
{
	class CommandList;
	class Fence;
	class CommandQueue
	{
	private:
		ComPtr<ID3D12CommandQueue> mD3DCmdQueue;
		D3D12_COMMAND_LIST_TYPE    mD3DType;
		Dictionary<CommandList*, UniquePtr<CommandList>>   mCmdLists;
		SortedDictionary<i32, List<CommandList*>> mExpectExcuteCmdLists;
		SortedDictionary<u64, Queue<UniquePtr<CommandList>>>     mPendingCmdLists;
		Queue<UniquePtr<CommandList>> mCurrentPendingCmdLists;
		UniquePtr<Fence> mFence;
		List<u64> mFenceValue;
	public:
		CommandQueue(u64 bufferCount, D3D12_COMMAND_LIST_TYPE type);
		~CommandQueue();
	public:
		CommandList* RequestCommandList(i32 priority);
		void Begin();
		void End();
		void Flush();

		ID3D12CommandQueue* Get() const {
			return mD3DCmdQueue.Get();
		}
	private:
		CommandList* RequestCommandList();
	};
}


