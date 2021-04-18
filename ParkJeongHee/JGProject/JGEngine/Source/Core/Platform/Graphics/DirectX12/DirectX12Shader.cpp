#include "pch.h"
#include "DirectX12Shader.h"
#include "Utill/PipelineState.h"
#include "Utill/RootSignature.h"
#include "Utill/CommandList.h"
#include "Utill/ShaderDataForm.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"

namespace JG
{
	bool DirectX12Shader::Compile(const String& sourceCode, EShaderFlags flags, String* error)
	{
		mFlags = flags;

		if (mFlags & EShaderFlags::Allow_ComputeShader)
		{
			if (mFlags & EShaderFlags::Allow_VertexShader ||
				mFlags & EShaderFlags::Allow_DomainShader ||
				mFlags & EShaderFlags::Allow_HullShader ||
				mFlags & EShaderFlags::Allow_GeometryShader ||
				mFlags & EShaderFlags::Allow_PixelShader)
			{
				if (error)
				{
					*error = TT("ComputeShader cannot contain (Vertex, Domain, Hull, Geometry, Pixel) Shader.");
				}
				return false;
			}
		}


		JG_CORE_INFO("{0} Compile Start", GetName());
		String code = sourceCode;
		if (mShaderDataForm == nullptr)
		{
			mShaderDataForm = CreateUniquePtr<ShaderDataForm>();
		}
		else
		{
			mShaderDataForm->Reset();
		}

		if (mShaderDataForm->Set(code) == false)
		{
			if (error != nullptr)
			{
				*error = TT("Failed Analysis SourceCode");
			}
			return false;
		}
		
		

		if (mFlags & EShaderFlags::Allow_ComputeShader)
		{
			ComputeCompile(code, error);
		}
		else
		{
			GraphicsCompile(code, error);
		}


		JG_CORE_INFO("{0} Compile Success", GetName());
	
		return true;
	}

	bool DirectX12Shader::Bind()
	{
		if (mIsCompileSuccess == false)
		{
			JG_CORE_ERROR("Failed Bind Shader : is not Compiled Shader ");
			return false;
		}
		auto RootSig = DirectX12API::GetRootSignature();
		RootSig->Reset();

		for (auto& rpPair : mShaderDataForm->RootParamMap)
		{
			u64 rootParam = rpPair.first;
			auto element = rpPair.second;

			switch (element->ElementType)
			{
			case HLSL::EHLSLElement::CBuffer:
				RootSig->InitAsCBV(element->RegisterNum, element->RegisterSpace);
				break;
			case HLSL::EHLSLElement::StructuredBuffer:
				RootSig->InitAsSRV(element->RegisterNum, element->RegisterSpace);
				break;
			case HLSL::EHLSLElement::RWStructuredBuffer:
				RootSig->InitAsUAV(element->RegisterNum, element->RegisterSpace);
				break;
			case HLSL::EHLSLElement::Texture:
				RootSig->InitAsDescriptorTable(
					D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
					(u32)(static_cast<ShaderDataForm::TextureData*>(element))->TextureCount,
					element->RegisterNum, element->RegisterSpace);
				break;
			case HLSL::EHLSLElement::RWTexture:
				RootSig->InitAsDescriptorTable(
					D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
					(u32)(static_cast<ShaderDataForm::TextureData*>(element))->TextureCount,
					element->RegisterNum, element->RegisterSpace);
				break;
			case HLSL::EHLSLElement::SamplerState:
				JGASSERT("IS NOT IMPL");
				break;
			}
		}

		for (auto& _pair : mShaderDataForm->SamplerStateDataMap)
		{
			RootSig->AddStaticSamplerState(_pair.second->Desc);
		}



		if (RootSig->Finalize() == false)
		{
			JG_CORE_ERROR("Failed Bind Shader : Failed Create RootSignature");
			return false;
		}
		
		if (mFlags & EShaderFlags::Allow_ComputeShader)
		{
			ComputeBind(RootSig);
		}
		else
		{
			GraphicsBind(RootSig);
		}
		

		return true;
	}

	void DirectX12Shader::SetName(const String& name)
	{
		mName = name;
	}

	const String& DirectX12Shader::GetName() const
	{
		return mName;
	}

	bool DirectX12Shader::GraphicsCompile(const String& code, String* error)
	{
		if (mFlags & EShaderFlags::Allow_VertexShader)
		{
			if (Compile(mVSData, code, CompileConfig(HLSL::VSEntry, HLSL::VSTarget), error) == false)
			{
				return false;
			}
		}
		if (mFlags & EShaderFlags::Allow_DomainShader)
		{
			if (Compile(mDSData, code, CompileConfig(HLSL::DSEntry, HLSL::DSTarget), error) == false)
			{
				return false;
			}
		}
		if (mFlags & EShaderFlags::Allow_HullShader)
		{
			if (Compile(mHSData, code, CompileConfig(HLSL::HSEntry, HLSL::HSTarget), error) == false)
			{
				return false;
			}
		}
		if (mFlags & EShaderFlags::Allow_GeometryShader)
		{
			if (Compile(mGSData, code, CompileConfig(HLSL::GSEntry, HLSL::GSTarget), error) == false)
			{
				return false;
			}
		}
		if (mFlags & EShaderFlags::Allow_PixelShader)
		{
			if (Compile(mPSData, code, CompileConfig(HLSL::PSEntry, HLSL::PSTarget), error) == false)
			{
				return false;
			}
		}

		return true;
	}

	void DirectX12Shader::GraphicsBind(SharedPtr<RootSignature> RootSig)
	{
		auto commandList = DirectX12API::GetGraphicsCommandList();
		commandList->BindRootSignature(RootSig);


		auto PSO = DirectX12API::GetGraphicsPipelineState();
		PSO->BindRootSignature(*RootSig);
		PSO->BindShader(*this);
	}

	bool DirectX12Shader::ComputeCompile(const String& code, String* error)
	{
		if (mFlags & EShaderFlags::Allow_ComputeShader)
		{
			if (Compile(mCSData, code, CompileConfig(HLSL::CSEntry, HLSL::CSTarget), error) == false)
			{
				return false;
			}
		}
		return false;
	}

	void DirectX12Shader::ComputeBind(SharedPtr<RootSignature> RootSig)
	{
		// TODO 
		// Bind
		auto commandList = DirectX12API::GetComputeCommandList();
		commandList->BindRootSignature(RootSig);


		auto PSO = DirectX12API::GetComputePipelineState();
		PSO->BindRootSignature(*RootSig);
		PSO->BindShader(*this);
	}

	bool DirectX12Shader::Compile(ComPtr<ID3DBlob>& blob, const String& sourceCode, const CompileConfig& config, String* error)
	{
		ComPtr<ID3DBlob> errorData;
		HRESULT hr = D3DCompile2(
			ws2s(sourceCode).data(),
			sourceCode.size(),
			nullptr,
			nullptr,
			nullptr,
			ws2s(config.Entry).c_str(),
			ws2s(config.Target).c_str(),
			0, 0, 0, nullptr, 0,
			blob.GetAddressOf(),
			errorData.GetAddressOf()); 

		if (FAILED(hr) && error != nullptr)
		{
			*error = s2ws((char*)errorData->GetBufferPointer());
			return false;
		}
		mIsCompileSuccess = true; 
		return true;
	}
}

