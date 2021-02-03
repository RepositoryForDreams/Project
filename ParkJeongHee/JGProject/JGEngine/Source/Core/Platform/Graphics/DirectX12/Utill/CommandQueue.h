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
		std::unordered_map<CommandList*, UniquePtr<CommandList>>   mCmdLists;
		std::map<i32, std::vector<CommandList*>> mExpectExcuteCmdLists;
		std::map<u64, std::queue<UniquePtr<CommandList>>>     mPendingCmdLists;
		UniquePtr<Fence> mFence;
		std::vector<u64> mFenceValue;
	public:
		CommandQueue(u64 bufferCount, D3D12_COMMAND_LIST_TYPE type);
		~CommandQueue();
	public:
		CommandList* RequestCommandList(int priority);
		void Begin();
		void End();
		void Flush();
	private:
		CommandList* RequestCommandList();
	};
}


