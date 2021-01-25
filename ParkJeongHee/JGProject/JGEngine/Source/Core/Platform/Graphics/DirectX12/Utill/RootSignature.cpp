#include "pch.h"
#include "RootSignature.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
std::unordered_map<JG::u64, ComPtr<ID3D12RootSignature>> g_RootSigCahce;

namespace JG
{
	bool RootSignature::Finalize()
	{

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc((u32)m_RootParams.size(),
			m_RootParams.data(), (u32)m_SamplerState.size(), m_SamplerState.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		u64 HashCode = HashState(&rootSigDesc.Flags);

		HashCode = HashState(rootSigDesc.pStaticSamplers, m_SamplerState.size(), HashCode);



		for (u32 param = 0; param < (u32)m_RootParams.size(); ++param)
		{
			const D3D12_ROOT_PARAMETER& RootParam = rootSigDesc.pParameters[param];

			if (RootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				HashCode = HashState(RootParam.DescriptorTable.pDescriptorRanges,
					RootParam.DescriptorTable.NumDescriptorRanges, HashCode);

			}
			else
				HashCode = HashState(&RootParam, 1, HashCode);
		}

		ID3D12RootSignature** RSRef = nullptr;
		bool firstCompile = false;
		{
			static std::mutex s_HashMapMutex;
			std::lock_guard<std::mutex> CS(s_HashMapMutex);
			auto iter = g_RootSigCahce.find(HashCode);

			// Reserve space so the next inquiry will find that someone got here first.
			if (iter == g_RootSigCahce.end())
			{
				RSRef = g_RootSigCahce[HashCode].GetAddressOf();
				firstCompile = true;
			}
			else
				RSRef = iter->second.GetAddressOf();
		}

		if (firstCompile)
		{
			m_D3D_RootSig = CreateD3DRootSignature(DirectX12API::GetD3DDevice(), &rootSigDesc);
			if (m_D3D_RootSig == nullptr) assert("failed CreateRootSig");
			g_RootSigCahce[HashCode] = m_D3D_RootSig.Get();
		}
		else
		{
			while (*RSRef == nullptr)
				std::this_thread::yield();
			m_D3D_RootSig = *RSRef;
		}

		m_DescriptorRanges.clear();
		return true;
	}
}