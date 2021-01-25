#pragma once


#include "JGCore.h"
#include "DirectX12Helper.h"


namespace JG
{
	class RootSignature;
	/* 
	 * RequestDescriptorHeap 요거 굳이 나중에 안하고, 실시간으로 numDescriptor 갯수 알아서  Resize 하는 방식으로 ㄱ 
	 *
	 * 
	 */
	class DynamicDescriptorAllocator
	{
	public:
		DynamicDescriptorAllocator(uint32_t numDescriptor = 1024);
	public:
		void CommitRootSignature(RootSignature& rootSig);
		void CommitDescriptorTable(uint32_t rootParam, const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles);

		void Reset();
		void PushDescriptorTable(ComPtr<ID3D12GraphicsCommandList> d3dCmdList, ComPtr<ID3D12DescriptorHeap> d3dDescriptorHeap, bool is_graphics);
		int  GetDescriptorInitAsType(uint32_t rootParam) const {
			if (m_RootParamInitTypeMap.find(rootParam) == m_RootParamInitTypeMap.end())
				assert("DynamicDescriptorAllocator::GetDescriptorInitAsType  :  NonExistent rootParam");
			return m_RootParamInitTypeMap.at(rootParam);
		}
		D3D12_DESCRIPTOR_RANGE_TYPE GetDescriptorTableType(uint32_t rootParam) const {
			if (m_DescriptorTableType.find(rootParam) == m_DescriptorTableType.end())
				assert("DynamicDescriptorAllocator::GetDescriptorTableType  :  NonExistent rootParam");
			return m_DescriptorTableType.at(rootParam);
		}
	private:
		void RequestDescriptorHeap();
	private:
		struct CPUCache
		{
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> cpuHandles;
			uint32_t numDescriptor = 0;
		};
		struct D3DHeap
		{
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DescriptorHeap;
			uint32_t numDescriptor = 1024;
		};
	private:
		D3DHeap            m_CurrentD3DHeap;
		std::map<int, int> m_RootParamInitTypeMap;
		std::map<uint32_t, D3D12_DESCRIPTOR_RANGE_TYPE> m_DescriptorTableType;
		std::map<int, CPUCache>           m_CPUCache;
		uint32_t m_PushedHandleOffset = 0;
		bool     m_ResizeDirty = false;
		uint32_t m_IncreaseSize = 0;
	};
}

