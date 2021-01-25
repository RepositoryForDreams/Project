#pragma once

#include "JGCore.h"
#include "DirectX12Helper.h"



namespace JG
{
	class RootSignature
	{
		friend class DynamicDescriptorAllocator;
	public:
		enum
		{
			__DescriptorTable__,
			__ShaderResourceView__,
			__UnorderedAccessView__,
			__ConstantBufferView__,
			__Constant__
		};
		struct DescriptorTableInfo
		{
			D3D12_DESCRIPTOR_RANGE_TYPE type;
			u32 numDescirptor;
		};
	public:  // 0, 1, 2, 3, 4, 5,
		// 000
		void InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE type, u32 numDescriptor, u32 register_num,
			u32 register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			u32 rootParam = (u32)m_RootSigInitType.size();
			DescriptorTableInfo info;
			info.numDescirptor = numDescriptor;
			info.type = type;
			m_DescriptorTableInfoByRootParam[rootParam] = info;

			auto range = std::make_shared<CD3DX12_DESCRIPTOR_RANGE>();
			range->Init(type, numDescriptor, register_num, register_space);

			CD3DX12_ROOT_PARAMETER param;
			param.InitAsDescriptorTable(1, range.get(), visibility);
			m_RootParams.push_back(param);
			m_DescriptorRanges.push_back(range);

			m_RootSigInitType.push_back(__DescriptorTable__);
		}
		void InitAsSRV(u32 register_num, u32 register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			CD3DX12_ROOT_PARAMETER param;

			param.InitAsShaderResourceView(register_num, register_space, visibility);
			m_RootParams.push_back(param);



			m_RootSigInitType.push_back(__ShaderResourceView__);
		}
		void InitAsUAV(u32 register_num, u32 register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			CD3DX12_ROOT_PARAMETER param;

			param.InitAsUnorderedAccessView(register_num, register_space, visibility);
			m_RootParams.push_back(param);

			m_RootSigInitType.push_back(__UnorderedAccessView__);
		}
		void InitAsCBV(u32 register_num, u32 register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			CD3DX12_ROOT_PARAMETER param;

			param.InitAsConstantBufferView(register_num, register_space, visibility);
			m_RootParams.push_back(param);

			m_RootSigInitType.push_back(__ConstantBufferView__);
		}
		void InitAsConstant(u32 btSize, u32 register_num, u32 register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			CD3DX12_ROOT_PARAMETER param;

			param.InitAsConstants(btSize / 4, register_num, register_space, visibility);
			m_RootParams.push_back(param);

			m_RootSigInitType.push_back(__Constant__);
		}
		void AddStaticSamplerState(const CD3DX12_STATIC_SAMPLER_DESC& desc)
		{
			m_SamplerState.push_back(desc);
		}
		DescriptorTableInfo GetDescriptorTableRangeType(u32 rootparam) const {
			if (m_DescriptorTableInfoByRootParam.find(rootparam) == m_DescriptorTableInfoByRootParam.end())
				assert("GetDescriptorTableInfo : nonexist DescriptorTable in rootparam");
			else
			{
				return m_DescriptorTableInfoByRootParam.at(rootparam);
			}
			return DescriptorTableInfo();
		}
		bool Finalize();
	public:
		ID3D12RootSignature* GetD3DPtr() const {
			return m_D3D_RootSig.Get();
		}
	private:
		ComPtr<ID3D12RootSignature> m_D3D_RootSig;
		std::vector<CD3DX12_ROOT_PARAMETER>         m_RootParams;
		std::vector<D3D12_STATIC_SAMPLER_DESC>      m_SamplerState;
		std::vector<i32> m_RootSigInitType;
		std::map<u32, DescriptorTableInfo> m_DescriptorTableInfoByRootParam;
		std::vector<std::shared_ptr<D3D12_DESCRIPTOR_RANGE>> m_DescriptorRanges;
	};
}



