#include "pch.h"
#include "PipelineState.h"
#include "RootSignature.h"

namespace JG
{
	DXGI_FORMAT ConvertShaderDataType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::_float: return DXGI_FORMAT_R32_FLOAT;
		case ShaderDataType::_float2: return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataType::_float3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::_float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;

		case ShaderDataType::_int: return  DXGI_FORMAT_R32_SINT;
		case ShaderDataType::_int2: return DXGI_FORMAT_R32G32_SINT;
		case ShaderDataType::_int3: return DXGI_FORMAT_R32G32B32_SINT;
		case ShaderDataType::_int4: return DXGI_FORMAT_R32G32B32A32_SINT;

		case ShaderDataType::_uint: return DXGI_FORMAT_R32_UINT;
		case ShaderDataType::_uint2: return DXGI_FORMAT_R32G32_UINT;
		case ShaderDataType::_uint3: return DXGI_FORMAT_R32G32B32_UINT;
		case ShaderDataType::_uint4: return DXGI_FORMAT_R32G32B32A32_UINT;

		default:
			JG_CORE_ERROR("ConvertShaderDataType :: not supported ShaderDataType");
			return DXGI_FORMAT_UNKNOWN;
		}
		

	}


	void GraphicsPipelineState::BindRootSignature(const RootSignature& rootSig)
	{
		mDesc.pRootSignature = rootSig.Get();
	}

	void GraphicsPipelineState::BindRenderTarget(const std::vector<DXGI_FORMAT>& rtFormats, DXGI_FORMAT dvFormat)
	{
		u64 cnt = rtFormats.size();
		if (cnt >= MAX_RENDERTARGET)
		{
			JG_CORE_ERROR("RenderTarget Num can not exceed 8");
			cnt = MAX_RENDERTARGET;
		}


		mDesc.NumRenderTargets = (u32)cnt;
		for (u64 i = 0; i < cnt; ++i)
		{
			mDesc.RTVFormats[i] = rtFormats[i];
		}
	}

	void GraphicsPipelineState::BindInputLayout(SharedPtr<InputLayout> inputLayout)
	{
		u32 offset = 0;
		inputLayout->ForEach([&](const InputElement& element)
		{
			D3D12_INPUT_ELEMENT_DESC Desc = {};
			Desc.SemanticIndex = element.SementicSlot;
			Desc.SemanticName = element.SementicName;
			Desc.Format = ConvertShaderDataType(element.Type);
			Desc.InputSlot = 0;
			Desc.AlignedByteOffset = offset;
			Desc.InstanceDataStepRate = 0;
			Desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			mD3DInputLayoutDescs.push_back(Desc);
			offset += (u32)GetShaderDataTypeSize(element.Type);
		});

		mDesc.InputLayout.NumElements        = (u32)mD3DInputLayoutDescs.size();
		mDesc.InputLayout.pInputElementDescs = mD3DInputLayoutDescs.data();

	}

	void GraphicsPipelineState::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
	{
		mDesc.PrimitiveTopologyType = type;
	}

	void GraphicsPipelineState::SetSampleMask(uint32_t sampleMask)
	{
		mDesc.SampleMask = sampleMask;
	}

	void GraphicsPipelineState::SetRasterizerState(const D3D12_RASTERIZER_DESC& desc)
	{
		mDesc.RasterizerState = desc;
	}

	void GraphicsPipelineState::SetBlendState(const D3D12_BLEND_DESC& desc)
	{
		mDesc.BlendState = desc;
	}

	void GraphicsPipelineState::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc)
	{
		mDesc.DepthStencilState = desc;
	}

	bool GraphicsPipelineState::Finalize()
	{
		return false;
	}

}