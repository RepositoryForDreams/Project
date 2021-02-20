#pragma once
#include "JGCore.h"
#include "DirectX12Helper.h"
#include "Graphics/InputLayout.h"

namespace JG
{
	class RootSignature;
	
	//class 
	class PipelineState
	{
	protected:
		ComPtr<ID3D12PipelineState> mD3DPSO;
	public:
		ID3D12PipelineState* Get() const {
			return mD3DPSO.Get();
		}
		virtual bool Finalize() = 0;
	};

	class GraphicsPipelineState : public PipelineState
	{

	public:
		void BindRootSignature(const RootSignature& rootSig);
		void BindRenderTarget(const List< DXGI_FORMAT>& rtFormats, DXGI_FORMAT dvFormat = DXGI_FORMAT_UNKNOWN);
		void BindInputLayout(SharedPtr<InputLayout> inputLayout);
		//void BindShader(ShaderType type, const Shader& shader);
		void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
		void SetSampleMask(uint32_t sampleMask);
		void SetRasterizerState(const D3D12_RASTERIZER_DESC& desc);
		void SetBlendState(const D3D12_BLEND_DESC& desc);
		void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc);

		virtual bool Finalize() override;
	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC mDesc = {};
		List<D3D12_INPUT_ELEMENT_DESC> mD3DInputLayoutDescs;
		//InputLayout m_InputLayout;

	};

	class ComputePipelineState
	{

	};
}