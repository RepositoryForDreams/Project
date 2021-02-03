#pragma once

#include "JGCore.h"
#include "DirectX12Helper.h"

namespace JG
{
	class ResourceStateTracker;
	class UploadAllocator;
	class CommandList
	{
	private:
		ComPtr<ID3D12PipelineState> mBindedGraphicsPSO = nullptr;
		ComPtr<ID3D12PipelineState> mBindedComputePSO = nullptr;
		ComPtr<ID3D12RootSignature> mBindedGraphicsRootSig = nullptr;
		ComPtr<ID3D12RootSignature> mBindedComputeRootSig = nullptr;

		ComPtr<ID3D12DescriptorHeap> mBindedDescriptorHeap = nullptr;

		D3D12_COMMAND_LIST_TYPE           mD3DType;
		ComPtr<ID3D12GraphicsCommandList> mD3DCommandList;
		ComPtr<ID3D12CommandAllocator>    mD3DAllocator;

		std::vector<ComPtr<ID3D12Object>> mTempObjectList;

		UniquePtr<UploadAllocator>		mUploadAllocator;
		UniquePtr<ResourceStateTracker> mResourceStateTracker;
	public:
		CommandList(D3D12_COMMAND_LIST_TYPE d3dType);
		~CommandList();
	public:
		ID3D12GraphicsCommandList* Get() const {
			return mD3DCommandList.Get();
		}
		void BackupResource(ID3D12Object* d3dObj);
		void Reset();
		void Close();
		bool Close(CommandList* commandList);
	};
	
}

