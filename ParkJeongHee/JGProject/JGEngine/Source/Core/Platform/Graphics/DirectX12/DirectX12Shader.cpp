#include "pch.h"
#include "DirectX12Shader.h"
#include "Utill/PipelineState.h"
#include "Utill/RootSignature.h"
#include "Utill/CommandList.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"

namespace JG
{
	bool DirectX12Shader::Compile(const String& sourceCode, EShaderFlags flags, String* error)
	{
		JG_CORE_INFO("{0} Compile Start", ws2s(GetName()));
		String code = sourceCode;
		if (mShaderData == nullptr)
		{
			mShaderData = CreateUniquePtr<DirectX12ShaderData>();
		}
		else
		{
			mShaderData->Reset();
		}

		if (mShaderData->Set(code) == false)
		{
			if (error != nullptr)
			{
				*error = TT("Failed Analysis SourceCode");
			}
			return false;
		}

		
		bool result = true;
		if (flags & EShaderFlags::Allow_VertexShader)
		{
			if (Compile(mVSData, code, CompileConfig(ShaderCode::HLSL::VSEntry, ShaderCode::HLSL::VSTarget), error) == false)
			{
				return false;
			}
		}
		if (flags & EShaderFlags::Allow_DomainShader)
		{
			if (Compile(mDSData, code, CompileConfig(ShaderCode::HLSL::DSEntry, ShaderCode::HLSL::DSTarget), error) == false)
			{
				return false;
			}
		}
		if (flags & EShaderFlags::Allow_HullShader)
		{
			if (Compile(mHSData, code, CompileConfig(ShaderCode::HLSL::HSEntry, ShaderCode::HLSL::HSTarget), error) == false)
			{
				return false;
			}
		}
		if (flags & EShaderFlags::Allow_GeometryShader)
		{
			if (Compile(mGSData, code, CompileConfig(ShaderCode::HLSL::GSEntry, ShaderCode::HLSL::GSTarget), error) == false)
			{
				return false;
			}
		}
		if (flags & EShaderFlags::Allow_PixelShader)
		{
			if (Compile(mPSData, code, CompileConfig(ShaderCode::HLSL::PSEntry, ShaderCode::HLSL::PSTarget), error) == false)
			{
				return false;
			}
		}




		// ShaderCompile



		/*
		2d에서는 머터리얼이 필요가 없다.

		Material::Create(Shader)    -> Material 생성 시 Shader Compile
		MaterialInstance(Matedrial) -> Compile할 필요가 없다.
		
		
		*/


		/* 렌더링 절차
		* 공통
		* RootSignature 바인딩
		* 
		* 개인
		* pso 바인딩
		* Texture, CBuffer, Structured Buffer 각종 바인딩
		* Vertex, IndexBuffer  바인딩 (Mesh 란 ? Material 여러개 붙일수 있다. SubMesh를 붙인다는 느낌으로 IndexBuffer 은 하나만 받는다.) -> Bind를 하게되면 VertexBuffer 를 모두 합친다.
		* 
		* 
		* 2D로 가게되면
		* Mesh 하나로 그린다.
		* 그릴때 마다 Mesh에 추가 Index추가
		* 텍스쳐는 하나의 배열로 올리고 상수값으로 textureIndex로 바인딩 한다.
		* vertexbuffer->setdata로 올리기.
		
		
		
		*/




		JG_CORE_INFO("{0} Compile Success", ws2s(GetName()));
		mIsCompileSuccess = true;
		return true;
	}

	bool DirectX12Shader::Bind()
	{
		if (mIsCompileSuccess == false)
		{
			JG_CORE_ERROR("Failed Bind Shader : is not Compiled Shader ");
			return false;
		}
		// RootSignature 수정

		auto RootSig = DirectX12API::GetRootSignature();
		RootSig->Reset();

		for (auto& rpPair : mShaderData->RootParamMap)
		{
			u64 rootParam = rpPair.first;
			auto element = rpPair.second;

			switch (element->ShaderElementType)
			{
			case DirectX12ShaderData::EShaderElementType::CBuffer:
				RootSig->InitAsCBV(element->RegisterNum, element->RegisterSpace);
				break;
			case DirectX12ShaderData::EShaderElementType::StructuredBuffer:
				RootSig->InitAsSRV(element->RegisterNum, element->RegisterSpace);
				break;
			case DirectX12ShaderData::EShaderElementType::Texture:
				RootSig->InitAsDescriptorTable(
					D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
					(u32)(static_cast<DirectX12ShaderData::TextureData*>(element))->TextureCount,
					element->RegisterNum, element->RegisterSpace);
				break;
			case DirectX12ShaderData::EShaderElementType::SamplerState:
				JGASSERT("IS NOT IMPL");
				break;
			}
		}

		for (auto& _pair : mShaderData->SamplerStateDataMap)
		{
			RootSig->AddStaticSamplerState(_pair.second->Desc);
		}



		if (RootSig->Finalize() == false)
		{
			JG_CORE_ERROR("Failed Bind Shader : Failed Create RootSignature");
			return false;
		}
		

		auto commandList = DirectX12API::GetGraphicsCommandList();
		commandList->BindRootSignature(RootSig);



		auto PSO = DirectX12API::GetGraphicsPipelineState();
		// PSO 수정
		PSO->BindRootSignature(*RootSig);
		PSO->BindShader(*this);

		// InputLayout -> VertexBuffer
		// RootSig (o)
		// Shader  (o)
		// RenderTarget -> SetRenderTarget
		// 
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
		return true;
	}



	bool DirectX12ShaderData::Set(String& code)
	{
		bool result = true;
		Reset();

		u64 pos = 0;
		// Upload Data 추출
		while (pos != String::npos)
		{
			pos = AnalysisCBuffer(code, pos, &result);
		}
		// StructuredBuffer
		pos = 0;
		while (pos != String::npos)
		{
			pos = AnalysisStructuredBuffer(code, pos, &result);
		}


		// Texture
		pos = 0;
		while (pos != String::npos)
		{
			pos = AnalysisTexture2D(code, pos, &result);
		}


		// SamplerState
		pos = 0;
		while (pos != String::npos)
		{
			pos = AnalysisSamplerState(code, pos, &result);
		}
		

		// Sampler는 이미 저

		return result;
	}
	void DirectX12ShaderData::Reset()
	{
		RootParamOffset = 0; 
		SpaceOffset = 0;
		TextureRegisterNumberOffset = 0;
		TextureCubeRegisterNumberOffset = 0;
		SamplerStateRegisterNumberOffset = 0;
		RootParamMap.clear();
		CBufferVarMap.clear();
		CBufferDataMap.clear();
		TextureDataMap.clear();
		SamplerStateDataMap.clear();
	}

	u64 DirectX12ShaderData::AnalysisCBuffer(const String& code, u64 startPos, bool* result)
	{
		CBufferData* cBuffer = nullptr;
		u64 uploadDataSize = 0;
		u64 dataTokenStartPos = code.find(ShaderCode::HLSL::CBToken, startPos);

		if (dataTokenStartPos != String::npos)
		{
			u64 startPos = dataTokenStartPos + wcslen(ShaderCode::HLSL::CBToken);
			u64 endPos = code.find_first_of(TT("{"), startPos);
			String cbName = code.substr(startPos, endPos - startPos);
			cbName = ReplaceAll(cbName, TT("\n"), TT(""));
			cbName = ReplaceAll(cbName, TT("\t"), TT(""));
			cbName = ReplaceAll(cbName, TT(" "), TT(""));
			if (RegisterCBuffer(cbName) == false)
			{
				if (result != nullptr)
				{
					*result = false;
				}
				// Error
				//
				return String::npos;
			}
			cBuffer = CBufferDataMap[cbName].get();
			cBuffer->Name = cbName;
		}
		else
		{
			return String::npos;
		}

		// Var 처리
		dataTokenStartPos = code.find_first_of(TT("{"), dataTokenStartPos) + 1;
		if (dataTokenStartPos != String::npos)
		{
			u64 dataTokenEndPos = code.find_first_of(TT("}"), dataTokenStartPos);
			String dataCode = code.substr(dataTokenStartPos, dataTokenEndPos - dataTokenStartPos);

			u64 pos = 0;
			while (pos != String::npos)
			{
				String varCode;
				pos = ExtractCBufferVar(dataCode, pos, &varCode);
				if (pos == String::npos)
					break;
				if (RegisterCBufferVar(cBuffer, varCode, uploadDataSize) == false)
				{
					if (result != nullptr)
					{
						*result = false;
					}
					return String::npos;
				}
			}

			startPos = dataTokenEndPos + 1;
		}
		else
		{
			return String::npos;
		}

		cBuffer->DataSize = uploadDataSize;
		cBuffer->ShaderElementType = EShaderElementType::CBuffer;
		cBuffer->RootParm = RootParamOffset++;
		cBuffer->RegisterNum = (u32)CBufferDataMap.size() - 1;
		RootParamMap[cBuffer->RootParm] = cBuffer;
		return startPos;
	}
	u64 DirectX12ShaderData::AnalysisStructuredBuffer(String& code, u64 startPos, bool* result)
	{

		u64 dataTokenStartPos = code.find(ShaderCode::HLSL::SBToken, startPos);
		if(dataTokenStartPos != String::npos)
		{
			u64 endPos = code.find(TT(";"), dataTokenStartPos);

			String dataCode = code.substr(dataTokenStartPos, endPos - dataTokenStartPos);


			// Type
			u64 dataTypeStartPos = dataCode.find(TT("<")) + 1;
			u64 dataTypeEndPos   = dataCode.find(TT(">"));



			String typeCode = dataCode.substr(dataTypeStartPos, dataTypeEndPos - dataTypeStartPos);
			typeCode = ReplaceAll(typeCode, TT(" "), TT(""));



			String nameCode = dataCode.substr(dataTypeEndPos + 1, dataCode.length() - dataTypeEndPos - 1);
			nameCode = ReplaceAll(nameCode, TT(" "), TT(""));

			if (RegisterStructuredBuffer(nameCode) == false)
			{
				if (result != nullptr)
				{
					*result = false;
				}
				return String::npos;
			}

			auto structuredBufferData = StructuredBufferDataMap[nameCode].get();
			structuredBufferData->Type = StringToShaderDataType(typeCode);
			if (structuredBufferData->Type == EShaderDataType::unknown)
			{
				JGASSERT("Not Supported Custom Struct Data in StructuredBuffer");
			}
	
			structuredBufferData->Name = nameCode;
			structuredBufferData->RootParm = RootParamOffset++;
			structuredBufferData->RegisterNum = 0;
			structuredBufferData->RegisterSpace = (u32)(DirectX12ShaderData::StructuredBufferStartSpace + SpaceOffset++);
			structuredBufferData->ShaderElementType = EShaderElementType::StructuredBuffer;
			structuredBufferData->ElementDataSize   = GetShaderDataTypeSize(structuredBufferData->Type);
			RootParamMap[structuredBufferData->RootParm] = structuredBufferData;


			
			code.insert(endPos, TT(" : register(t0, space") + std::to_wstring(structuredBufferData->RegisterSpace) + TT(")"));
			startPos = endPos + 1;
		}
		else
		{
			return String::npos;
		}
		return startPos;
	}
	u64 DirectX12ShaderData::AnalysisTexture2D(String& code, u64 startPos, bool* result)
	{

		u64 dataTokenStartPos = code.find(ShaderCode::HLSL::Texture2DToken, startPos);
		if (dataTokenStartPos != String::npos)
		{
			u64 endPos = code.find(TT(";"), dataTokenStartPos);

			String dataCode = code.substr(dataTokenStartPos, endPos - dataTokenStartPos);


			String nameCode = ReplaceAll(dataCode, ShaderCode::HLSL::Texture2DToken, TT(""));
			nameCode = ReplaceAll(nameCode, TT(" "), TT(""));

			u64 arraySize = 1;

			u64 arrayStartPos = dataCode.find(TT("["));
			if (arrayStartPos != String::npos)
			{

				nameCode = nameCode.substr(0, nameCode.find(TT("[")));



				arrayStartPos += 1;
				u64 arrayEndPos = dataCode.find(TT("]", arrayStartPos));


				String arraySizeCode = dataCode.substr(arrayStartPos, arrayEndPos - arrayStartPos);
				arraySizeCode = ReplaceAll(arraySizeCode, TT(" "), TT(""));

				arraySize = _wtoi64(arraySizeCode.c_str());
			}

			if (RegisterTextureData(nameCode) == false)
			{
				if (result != nullptr)
				{
					*result = false;
				}
				return String::npos;
			}

			auto textureData = TextureDataMap[nameCode].get();
			textureData->Name = nameCode;
			textureData->RootParm = RootParamOffset++;
			textureData->RegisterNum = TextureRegisterNumberOffset; TextureRegisterNumberOffset += arraySize;
			textureData->RegisterSpace = DirectX12ShaderData::Texture2DStartSpace;
			textureData->ShaderElementType = EShaderElementType::Texture;
			textureData->Type = ETextureType::_2D;
			textureData->TextureCount = arraySize;
			RootParamMap[textureData->RootParm] = textureData;


			code.insert(endPos,
				TT(" : register(t") + std::to_wstring(TextureDataMap[nameCode]->RegisterNum) +
				TT(", space") + std::to_wstring(TextureDataMap[nameCode]->RegisterSpace) + TT(")"));
			startPos = endPos + 1;
		}
		else
		{
			return String::npos;
		}
		return startPos;
	}
	u64 DirectX12ShaderData::AnalysisSamplerState(String& code, u64 startPos, bool* result)
	{
		// TODO SamplerState 분석

		u64 dataTokenStartPos = code.find(ShaderCode::HLSL::SamplerStateToken, startPos);

		if (dataTokenStartPos != String::npos)
		{
			u64 endPos = code.find(TT(";"), dataTokenStartPos);
			String dataCode = code.substr(dataTokenStartPos, endPos - dataTokenStartPos);
			u64 samplerDataStartPos = dataCode.find(TT("{"));

			String nameCode = dataCode.substr(0, samplerDataStartPos);
			nameCode = ReplaceAll(nameCode , ShaderCode::HLSL::SamplerStateToken, TT(""));
			nameCode = ReplaceAll(nameCode, TT(" "), TT(""));
			nameCode = ReplaceAll(nameCode, TT("\n"), TT(""));
			nameCode = ReplaceAll(nameCode, TT("\t"), TT(""));

			if (samplerDataStartPos != String::npos)
			{
				samplerDataStartPos += 1;
				u64 samplerDataEndPos = dataCode.find(TT("}"), samplerDataStartPos);
			
				String samplerDataCode = dataCode.substr(samplerDataStartPos, samplerDataEndPos - samplerDataStartPos) + TT(",");

				if (samplerDataCode.find_last_of(TT(",")) != samplerDataCode.length() - 1)
				{
					samplerDataCode += TT(",");
				}
				Dictionary<String, String> SamplerDataMap;

				u64 pos = 0;
				while (pos != String::npos)
				{
					String key; String value;
					pos = ExtractSamplerStateValue(samplerDataCode, pos, &key, &value);
					if (pos != String::npos)
					{
						SamplerDataMap[key] = value;
					}
				}

				if (RegisterSamplerStateData(nameCode) == false)
				{
					if (result != nullptr)
					{
						*result = false;
					}
					return String::npos;
				}

				auto samplerStateData = SamplerStateDataMap[nameCode].get();
				samplerStateData->Desc = CreateSamplerStateDesc(SamplerDataMap);


				samplerStateData->Name = nameCode;
				samplerStateData->RegisterNum = SamplerStateRegisterNumberOffset++;
				samplerStateData->Desc.ShaderRegister = samplerStateData->RegisterNum;
				samplerStateData->RegisterSpace = 0;
				samplerStateData->ShaderElementType = EShaderElementType::SamplerState;

				u64 replaceStartPos = code.find(TT("{"), dataTokenStartPos);
				u64 replaceEndPos = code.find(TT(";"), replaceStartPos) + 1;
				code = ReplaceAll(code, code.substr(replaceStartPos, replaceEndPos - replaceStartPos), TT(""));


				u64 insertStartPos = code.find(nameCode, dataTokenStartPos) + nameCode.length();
				String additionalCode = TT(" : register(s") + std::to_wstring(samplerStateData->RegisterNum) +
					TT(", space") + std::to_wstring(samplerStateData->RegisterSpace) + TT(");");

				code.insert(insertStartPos, additionalCode);
				endPos = insertStartPos + additionalCode.length();
			}
			else
			{
				if (result != nullptr)
				{
					*result = false;
				}
				return String::npos;
			}
			startPos = endPos + 1;
		}
		else
		{
			return String::npos;
		}
		return startPos;
	}
	u64 DirectX12ShaderData::ExtractCBufferVar(const String& code, u64 pos, String* out_value)
	{
		u64 startPos = (pos == 0) ? 0 : code.find_first_of(TT("\n"), pos) + 1;
		if (startPos != String::npos)
		{
			u64 endPos = code.find_first_of(TT(";"), startPos);
			if (endPos != String::npos)
			{
				String varCode = code.substr(startPos, endPos - startPos + 1);
				varCode = ReplaceAll(varCode, TT("\n"), TT(""));
				varCode = ReplaceAll(varCode, TT("\t"), TT(""));

				if (out_value)
				{
					*out_value = varCode;
				}
				else
				{
					return String::npos;
				}
				pos = endPos + 1;
				return pos;
			}
		}
		return String::npos;
	}
	u64 DirectX12ShaderData::ExtractSamplerStateValue(const String& samplerStateDataCode, u64 startPos, String* out_key, String* out_value)
	{
		auto endPos = samplerStateDataCode.find(TT(","), startPos);
		if (endPos == String::npos)
		{
			return String::npos;
		}

		u64 result = endPos + 1;

		String dataCode = samplerStateDataCode.substr(startPos, endPos - startPos);
		dataCode = ReplaceAll(dataCode, TT("\t"), TT(""));
		dataCode = ReplaceAll(dataCode, TT("\n"), TT(""));


		startPos = 0;
		u64 midPos = dataCode.find(TT("="));
		endPos = dataCode.length();
		
		if (out_key != nullptr)
		{
			*out_key = dataCode.substr(startPos, midPos - startPos);
			*out_key = ReplaceAll(*out_key, TT(" "), TT(""));
		}

		if (out_value != nullptr)
		{
			*out_value = dataCode.substr(midPos + 1, endPos - midPos - 1);
			*out_value = ReplaceAll(*out_value, TT(" "), TT(""));
		}


		return result;
	}
	D3D12_STATIC_SAMPLER_DESC DirectX12ShaderData::CreateSamplerStateDesc(const Dictionary<String, String>& samplerDataMap)
	{
		D3D12_STATIC_SAMPLER_DESC desc = {};

		//
		if (samplerDataMap.find(TT("Template")) != samplerDataMap.end())
		{
			CreateSamplerStateByTemplate(StringToSamplerStateTemplate(samplerDataMap.at(TT("Template"))), &desc);
		}
		else
		{
			desc = CD3DX12_STATIC_SAMPLER_DESC(0);
		}

		String Min = TT("Point"); 		String Mag = TT("Point"); 		String Mip = TT("Point");

		if (samplerDataMap.find(TT("Min")) != samplerDataMap.end())
		{
			Min = samplerDataMap.at(TT("Min"));
		}
		if (samplerDataMap.find(TT("Mag")) != samplerDataMap.end())
		{
			Mag = samplerDataMap.at(TT("Mag"));
		}
		if (samplerDataMap.find(TT("Mip")) != samplerDataMap.end())
		{
			Mip = samplerDataMap.at(TT("Mip"));
		}
		desc.Filter = GetSamplerStateFilter(Min, Mag, Mip);


		String AddressU = TT("Wrap"); String AddressV = TT("Wrap"); String AddressW = TT("Wrap");
		if (samplerDataMap.find(TT("AddressU")) != samplerDataMap.end())
		{
			AddressU = samplerDataMap.at(TT("AddressU"));
		}
		if (samplerDataMap.find(TT("AddressV")) != samplerDataMap.end())
		{
			AddressV = samplerDataMap.at(TT("AddressV"));
		}
		if (samplerDataMap.find(TT("AddressW")) != samplerDataMap.end())
		{
			AddressW = samplerDataMap.at(TT("AddressW"));
		}
		desc.AddressU = GetTextureAddressMode(AddressU);
		desc.AddressV = GetTextureAddressMode(AddressV);
		desc.AddressW = GetTextureAddressMode(AddressW);


		String ComparisonFunc = TT("ComparisonFunc");
		if (samplerDataMap.find(TT("AddressU")) != samplerDataMap.end())
		{
			ComparisonFunc = samplerDataMap.at(TT("ComparisonFunc"));
		}
		desc.ComparisonFunc = GetComparisonFunc(ComparisonFunc);


		String BorderColor = TT("BorderColor");
		if (samplerDataMap.find(TT("BorderColor")) != samplerDataMap.end())
		{
			BorderColor = samplerDataMap.at(TT("BorderColor"));
		}
		desc.BorderColor = GetBorderColor(BorderColor);


		if (samplerDataMap.find(TT("MinLOD")) != samplerDataMap.end())
		{
			desc.MinLOD = _wtof(samplerDataMap.at(TT("MinLOD")).c_str());
		}
		if (samplerDataMap.find(TT("MaxLOD")) != samplerDataMap.end())
		{
			desc.MaxLOD = _wtof(samplerDataMap.at(TT("MaxLOD")).c_str());
		}
		if (samplerDataMap.find(TT("MaxAnisotropy")) != samplerDataMap.end())
		{
			desc.MaxAnisotropy = _wtoi(samplerDataMap.at(TT("MaxAnisotropy")).c_str());
		}
		if (samplerDataMap.find(TT("MipLODBias")) != samplerDataMap.end())
		{
			desc.MipLODBias = _wtof(samplerDataMap.at(TT("MipLODBias")).c_str());
		}
		return desc;
	}

	void DirectX12ShaderData::CreateSamplerStateByTemplate(ESamplerStateTemplate _template, D3D12_STATIC_SAMPLER_DESC* out_desc)
	{
		if (out_desc == nullptr)
		{
			return;
		}
		if (_template == ESamplerStateTemplate::Unknown)
		{
			JG_CORE_WARN("Unknown Sampler Template");
		}
		switch (_template)
		{
		case ESamplerStateTemplate::Point_Wrap:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_POINT,
				D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
			break;
		case ESamplerStateTemplate::Point_Clamp:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_POINT,
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
			break;
		case ESamplerStateTemplate::Point_Border:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_POINT,
				D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER);
			break;
		case ESamplerStateTemplate::Point_Mirror:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_POINT,
				D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR);
			break;
		case ESamplerStateTemplate::Point_MirrorOnce:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_POINT,
				D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE, D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE, D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE);
			break;
		case ESamplerStateTemplate::Linear_Wrap:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_LINEAR,
				D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
			break;
		case ESamplerStateTemplate::Linear_Clamp:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_LINEAR,
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
			break;
		case ESamplerStateTemplate::Linear_Border:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_LINEAR,
				D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER);
			break;
		case ESamplerStateTemplate::Linear_Mirror:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_LINEAR,
				D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR);
			break;
		case ESamplerStateTemplate::Linear_MirrorOnce:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_LINEAR,
				D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE, D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE, D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE);
			break;
		case ESamplerStateTemplate::Anisotropic_Wrap:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_ANISOTROPIC,
				D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
			break;
		case ESamplerStateTemplate::Anisotropic_Clamp:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_ANISOTROPIC,
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
			break;
		case ESamplerStateTemplate::Anisotropic_Border:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_ANISOTROPIC,
				D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER);
			break;
		case ESamplerStateTemplate::Anisotropic_Mirror:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_ANISOTROPIC,
				D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR);
			break;
		case ESamplerStateTemplate::Anisotropic_MirrorOnce:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_ANISOTROPIC,
				D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE, D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE, D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE);
			break;
		default:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0);
			break;
		}
	}
	D3D12_FILTER DirectX12ShaderData::GetSamplerStateFilter(const String& Min, const String& Mag, const String& Mip)
	{
		enum
		{
			Point = 0,
			Linear = 1,
			Anisotropic = 2
		};
		i32 min = Point; i32 mag = Point; i32 mip = Point;


		if (Min == TT("Point")) min = Point;
		else if (Min == TT("Linear")) min = Linear;
		else if (Min == TT("Anisotropic")) min = Anisotropic;

		if (Mag == TT("Point")) mag = Point;
		else if (Mag == TT("Linear")) mag = Linear;
		else if (Mag == TT("Anisotropic")) mag = Anisotropic;

		if (Mip == TT("Point")) mip = Point;
		else if (Mip == TT("Linear")) mip = Linear;
		else if (Mip == TT("Anisotropic")) mip = Anisotropic;





		if (min == Point && mag == Point && mip == Point) return D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_POINT;
		else if (min == Point && mag == Point && mip == Linear) return D3D12_FILTER::D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		else if (min == Point && mag == Linear && mip == Point) return D3D12_FILTER::D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		else if (min == Linear && mag == Point && mip == Point) return D3D12_FILTER::D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		else if (min == Point && mag == Linear && mip == Linear) return D3D12_FILTER::D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		else if (min == Linear && mag == Point && mip == Linear) return D3D12_FILTER::D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		else if (min == Linear && mag == Linear && mip == Point) return D3D12_FILTER::D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		else if (min == Linear && mag == Linear && mip == Linear) return D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		else if (min == Anisotropic || mag == Anisotropic || mip == Anisotropic) return D3D12_FILTER::D3D12_FILTER_ANISOTROPIC;
		else return D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_POINT;
	}
	D3D12_TEXTURE_ADDRESS_MODE DirectX12ShaderData::GetTextureAddressMode(const String& addressMode)
	{
		if (addressMode == TT("Wrap")) return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		else if (addressMode == TT("Mirror")) return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		else if (addressMode == TT("Clamp")) return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		else if (addressMode == TT("Border")) return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		else if (addressMode == TT("MirrorOnce")) return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
		else return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	}
	D3D12_COMPARISON_FUNC DirectX12ShaderData::GetComparisonFunc(const String& comparisonFunc)
	{
		if (comparisonFunc == TT("Never")) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
		else if (comparisonFunc == TT("Less")) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
		else if (comparisonFunc == TT("Equal")) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_EQUAL;
		else if (comparisonFunc == TT("LessEqual")) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL;
		else if (comparisonFunc == TT("Greater")) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER;
		else if (comparisonFunc == TT("NotEqual")) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NOT_EQUAL;
		else if (comparisonFunc == TT("GreaterEqual")) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		else if (comparisonFunc == TT("Always")) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS;
		else return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL;
	}
	D3D12_STATIC_BORDER_COLOR DirectX12ShaderData::GetBorderColor(const String& borderColor)
	{
		// BorderColor = TransparentBlack, OpaqueBlack, OpaqueWhite
		if (borderColor == TT("TransparentBlack")) return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		else if (borderColor == TT("OpaqueBlack")) return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		else if (borderColor == TT("OpaqueWhite")) return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		else return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	}

	bool DirectX12ShaderData::RegisterStructuredBuffer(const String& name)
	{

		if (StructuredBufferDataMap.find(name) != StructuredBufferDataMap.end())
		{
			JG_CORE_ERROR("{0} StructuredBuffer Already Exists.", ws2s(name));
			return false;
		}
		StructuredBufferDataMap[name] = CreateUniquePtr<StructuredBufferData>();

		return true;
	}
	bool DirectX12ShaderData::RegisterCBuffer(const String& name)
	{

		if (CBufferDataMap.find(name) != CBufferDataMap.end())
		{
			JG_CORE_ERROR("{0} CBuffer Already Exists.", ws2s(name));
			return false;
		}

		CBufferDataMap[name] = CreateUniquePtr<CBufferData>();
		return true;
	}

	bool DirectX12ShaderData::RegisterTextureData(const String& name)
	{
		if (TextureDataMap.find(name) != TextureDataMap.end())
		{
			JG_CORE_ERROR("{0} TextureData Already Exists.", ws2s(name));
			return false;
		}

		TextureDataMap[name] = CreateUniquePtr<TextureData>();

		return true;
	}

	bool DirectX12ShaderData::RegisterCBufferVar(CBufferData* cBuffer, const String& varCode, u64& uploadDataSize)
	{
		u64 varStartPos = varCode.find_first_not_of(TT(" "), varCode.find_first_not_of(TT("\t")));
		u64 varMidPos = varCode.find(TT(" "), varStartPos);
		u64 varEndPos = varCode.find(TT(";"), varMidPos) - 1;




		String typeCode = varCode.substr(varStartPos, varMidPos - varStartPos);
		typeCode = ReplaceAll(typeCode, TT(" "), TT(""));
		String nameCode = varCode.substr(varMidPos + 1, varEndPos - varMidPos);
		nameCode = ReplaceAll(nameCode, TT(" "), TT(""));
		nameCode = ReplaceAll(nameCode, TT(";"), TT(""));





		cBuffer->DataMap[nameCode] = CreateUniquePtr<Data>();
		Data& uploadData    = *(cBuffer->DataMap[nameCode]);
		uploadData.Type     = StringToShaderDataType(typeCode);
		uploadData.DataSize = GetShaderDataTypeSize(uploadData.Type);
		uploadData.DataPos  = uploadDataSize;
		uploadData.Owner = cBuffer;
		uploadDataSize += uploadData.DataSize;


		if (CBufferVarMap.find(nameCode) != CBufferVarMap.end())
		{
			JG_CORE_ERROR("{0} CBuffer Var Already Exists.", ws2s(nameCode));
			return false;
		}
		CBufferVarMap[nameCode] = cBuffer->DataMap[nameCode].get();
		return true;
	}

	bool DirectX12ShaderData::RegisterSamplerStateData(const String& name)
	{
		if (SamplerStateDataMap.find(name) != SamplerStateDataMap.end())
		{
			JG_CORE_ERROR("{0} SamplerStateData Already Exists.", ws2s(name));
			return false;
		}

		SamplerStateDataMap[name] = CreateUniquePtr<SamplerStateData>();

		return true;
	}



}

