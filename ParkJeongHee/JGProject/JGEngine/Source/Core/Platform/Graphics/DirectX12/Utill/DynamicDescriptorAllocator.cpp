#include "pch.h"
#include "DynamicDescriptorAllocator.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
#include "RootSignature.h"

namespace JG
{
	DynamicDescriptorAllocator::DynamicDescriptorAllocator(uint32_t numDescriptor)
	{
		m_IncreaseSize = DirectX12API::GetD3DDevice()->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);
		RequestDescriptorHeap();
	}

	void DynamicDescriptorAllocator::CommitRootSignature(RootSignature& rootSig)
	{

		Reset();
		for (int i = 0; i < (int)rootSig.m_RootSigInitType.size(); ++i)
		{
			m_RootParamInitTypeMap[i] = rootSig.m_RootSigInitType[i];
		}

		for (auto& info_pair : rootSig.m_DescriptorTableInfoByRootParam)
		{
			uint32_t rootParam = info_pair.first;
			uint32_t numDescriptor = info_pair.second.numDescirptor;
			auto     rangeType = info_pair.second.type;

			m_DescriptorTableType[rootParam] = rangeType;
			m_CPUCache[rootParam].numDescriptor = numDescriptor;
		}
	}
	void DynamicDescriptorAllocator::CommitDescriptorTable(uint32_t rootParam, const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles)
	{
		auto& cpuHandles = m_CPUCache[rootParam].cpuHandles;
		cpuHandles.insert(cpuHandles.end(), handles.begin(), handles.end());
	}
	void DynamicDescriptorAllocator::Reset()
	{
		m_PushedHandleOffset = 0;
		m_RootParamInitTypeMap.clear();
		m_DescriptorTableType.clear();
		m_CPUCache.clear();
		RequestDescriptorHeap();
	}


	void DynamicDescriptorAllocator::PushDescriptorTable(ComPtr<ID3D12GraphicsCommandList> d3dCmdList, ComPtr<ID3D12DescriptorHeap> d3dDescriptorHeap, bool is_graphics)
	{
		if (m_ResizeDirty) return;
		if (d3dDescriptorHeap != m_CurrentD3DHeap.DescriptorHeap)
		{
			d3dDescriptorHeap = m_CurrentD3DHeap.DescriptorHeap;
			d3dCmdList->SetDescriptorHeaps(1, d3dDescriptorHeap.GetAddressOf());
		}
		if (!m_CPUCache.empty())
		{
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> cpu_Handles;
			CD3DX12_CPU_DESCRIPTOR_HANDLE startCPU(m_CurrentD3DHeap.DescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			CD3DX12_GPU_DESCRIPTOR_HANDLE startGPU(m_CurrentD3DHeap.DescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			for (auto& cache_pair : m_CPUCache)
			{
				int  rootParam = cache_pair.first;
				auto handles = cache_pair.second.cpuHandles.data();


				uint32_t handleCount = (uint32_t)cache_pair.second.cpuHandles.size();
				if (handleCount == 0) continue;
				auto gpu = startGPU.Offset(m_PushedHandleOffset, m_IncreaseSize);
				auto cpu = startCPU.Offset(m_PushedHandleOffset, m_IncreaseSize);
				m_PushedHandleOffset += handleCount;
				if (m_PushedHandleOffset >= m_CurrentD3DHeap.numDescriptor)
				{
					m_PushedHandleOffset = m_CurrentD3DHeap.numDescriptor;
					m_ResizeDirty = true;
				}

				std::vector<uint32_t> srcDescriptorRangeSize(handleCount, 1);

				DirectX12API::GetD3DDevice()->CopyDescriptors(
					1, &cpu, &handleCount,
					handleCount, handles, srcDescriptorRangeSize.data(),
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (is_graphics)
				{
					d3dCmdList->SetGraphicsRootDescriptorTable(rootParam, gpu);
				}
				else
				{
					d3dCmdList->SetComputeRootDescriptorTable(rootParam, gpu);
				}
				cache_pair.second.cpuHandles.clear();
			}

		}
	}
	void DynamicDescriptorAllocator::RequestDescriptorHeap()
	{
		if (m_CurrentD3DHeap.DescriptorHeap && m_ResizeDirty)
		{
			m_CurrentD3DHeap.numDescriptor *= 2;
			m_ResizeDirty = false;
			m_CurrentD3DHeap.DescriptorHeap = CreateD3DDescriptorHeap(DirectX12API::GetD3DDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, m_CurrentD3DHeap.numDescriptor);
		}
		else if (m_CurrentD3DHeap.DescriptorHeap == nullptr)
		{
			m_CurrentD3DHeap.DescriptorHeap = CreateD3DDescriptorHeap(DirectX12API::GetD3DDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, m_CurrentD3DHeap.numDescriptor);
		}
	}

	
}