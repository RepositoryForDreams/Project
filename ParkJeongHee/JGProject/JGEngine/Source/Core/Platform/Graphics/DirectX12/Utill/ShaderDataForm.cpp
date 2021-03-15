#include "pch.h"
#include "ShaderDataForm.h"

namespace JG
{
	bool ShaderDataForm::Set(String& code)
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
		// RWStructuredBuffer
		pos = 0;
		while (pos != String::npos)
		{
			pos = AnalysisRWStructuredBuffer(code, pos, &result);
		}

		// Texture
		pos = 0;
		while (pos != String::npos)
		{
			pos = AnalysisTexture2D(code, pos, &result);
		}
		// RWTexture
		pos = 0;
		{
			pos = AnalysisRWTexture2D(code, pos, &result);
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
	void ShaderDataForm::Reset()
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

	u64 ShaderDataForm::AnalysisCBuffer(const String& code, u64 startPos, bool* result)
	{
		CBufferData* cBuffer = nullptr;
		u64 uploadDataSize = 0;
		u64 dataTokenStartPos = code.find(HLSL::Token::CBuffer, startPos);

		if (dataTokenStartPos != String::npos)
		{
			String cbName;
			ExtractCBufferName(code, dataTokenStartPos, &cbName);
			if (RegisterCBuffer(cbName) == false)
			{
				if (result != nullptr)
				{
					*result = false;
				}
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
		cBuffer->ElementType = HLSL::EHLSLElement::CBuffer;
		cBuffer->RootParm = RootParamOffset++;
		cBuffer->RegisterNum = (u32)CBufferDataMap.size() - 1;
		RootParamMap[cBuffer->RootParm] = cBuffer;
		return startPos;
	}
	u64 ShaderDataForm::AnalysisStructuredBuffer(String& code, u64 startPos, bool* result)
	{

		u64 dataTokenStartPos = code.find(HLSL::Token::StructuredBuffer, startPos);
		if (dataTokenStartPos != String::npos)
		{
			u64 endPos = code.find(TT(";"), dataTokenStartPos);

			String dataCode = code.substr(dataTokenStartPos, endPos - dataTokenStartPos);


			// Type
			u64 dataTypeStartPos = dataCode.find(TT("<")) + 1;
			u64 dataTypeEndPos = dataCode.find(TT(">"));



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
			structuredBufferData->RegisterSpace = (u32)(ShaderDataForm::StructuredBufferStartSpace + SpaceOffset++);
			structuredBufferData->ElementType = HLSL::EHLSLElement::StructuredBuffer;
			structuredBufferData->ElementDataSize = GetShaderDataTypeSize(structuredBufferData->Type);
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
	u64 ShaderDataForm::AnalysisTexture2D(String& code, u64 startPos, bool* result)
	{

		u64 dataTokenStartPos = code.find(HLSL::Token::Texture2D, startPos);
		if (dataTokenStartPos != String::npos)
		{
			u64 endPos = code.find(TT(";"), dataTokenStartPos);

			String dataCode = code.substr(dataTokenStartPos, endPos - dataTokenStartPos);


			String nameCode = ReplaceAll(dataCode, HLSL::Token::Texture2D, TT(""));
			nameCode = ReplaceAll(nameCode, TT(" "), TT(""));

			u64 arraySize = 1;

			u64 arrayStartPos = dataCode.find(TT("["));
			if (arrayStartPos != String::npos)
			{

				nameCode = nameCode.substr(0, nameCode.find(TT("[")));



				arrayStartPos += 1;
				u64 arrayEndPos = dataCode.find(TT("]"), arrayStartPos);


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
			textureData->RegisterNum = (u32)TextureRegisterNumberOffset; TextureRegisterNumberOffset += arraySize;
			textureData->RegisterSpace = ShaderDataForm::Texture2DStartSpace;
			textureData->ElementType = HLSL::EHLSLElement::Texture;
			textureData->Type = HLSL::EHLSLTextureType::_2D;
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
	u64 ShaderDataForm::AnalysisSamplerState(String& code, u64 startPos, bool* result)
	{
		// TODO SamplerState 분석

		u64 dataTokenStartPos = code.find(HLSL::Token::SamplerState, startPos);

		if (dataTokenStartPos != String::npos)
		{
			u64 endPos = code.find(TT(";"), dataTokenStartPos);
			String dataCode = code.substr(dataTokenStartPos, endPos - dataTokenStartPos);
			u64 samplerDataStartPos = dataCode.find(TT("{"));

			String nameCode = dataCode.substr(0, samplerDataStartPos);
			nameCode = ReplaceAll(nameCode, HLSL::Token::SamplerState, TT(""));
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
				samplerStateData->RegisterNum = (u32)SamplerStateRegisterNumberOffset++;
				samplerStateData->Desc.ShaderRegister = samplerStateData->RegisterNum;
				samplerStateData->RegisterSpace = 0;
				samplerStateData->ElementType = HLSL::EHLSLElement::SamplerState;

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
	u64 ShaderDataForm::AnalysisRWStructuredBuffer(String& code, u64 startPos, bool* result)
	{
		u64 dataTokenStartPos = code.find(HLSL::Token::RWStructuredBuffer, startPos);
		if (dataTokenStartPos != String::npos)
		{
			u64 endPos = code.find(TT(";"), dataTokenStartPos);

			String dataCode = code.substr(dataTokenStartPos, endPos - dataTokenStartPos);


			// Type
			u64 dataTypeStartPos = dataCode.find(TT("<")) + 1;
			u64 dataTypeEndPos = dataCode.find(TT(">"));



			String typeCode = dataCode.substr(dataTypeStartPos, dataTypeEndPos - dataTypeStartPos);
			typeCode = ReplaceAll(typeCode, TT(" "), TT(""));



			String nameCode = dataCode.substr(dataTypeEndPos + 1, dataCode.length() - dataTypeEndPos - 1);
			nameCode = ReplaceAll(nameCode, TT(" "), TT(""));

			if (RegisterRWStructuredBuffer(nameCode) == false)
			{
				if (result != nullptr)
				{
					*result = false;
				}
				return String::npos;
			}

			auto structuredBufferData = RWStructuredBufferDataMap[nameCode].get();
			structuredBufferData->Type = StringToShaderDataType(typeCode);
			if (structuredBufferData->Type == EShaderDataType::unknown)
			{
				JGASSERT("Not Supported Custom Struct Data in StructuredBuffer");
			}

			structuredBufferData->Name = nameCode;
			structuredBufferData->RootParm = RootParamOffset++;
			structuredBufferData->RegisterNum = 0;
			structuredBufferData->RegisterSpace = (u32)(ShaderDataForm::StructuredBufferStartSpace + SpaceOffset++);
			structuredBufferData->ElementType = HLSL::EHLSLElement::RWStructuredBuffer;
			structuredBufferData->ElementDataSize = GetShaderDataTypeSize(structuredBufferData->Type);
			RootParamMap[structuredBufferData->RootParm] = structuredBufferData;



			code.insert(endPos, TT(" : register(u0, space") + std::to_wstring(structuredBufferData->RegisterSpace) + TT(")"));
			startPos = endPos + 1;
		}
		else
		{
			return String::npos;
		}
		return startPos;
	}
	u64 ShaderDataForm::AnalysisRWTexture2D(String& code, u64 startPos, bool* result)
	{
		u64 dataTokenStartPos = code.find(HLSL::Token::RWTexture2D, startPos);
		if (dataTokenStartPos != String::npos)
		{
			u64 endPos = code.find(TT(";"), dataTokenStartPos);

			String dataCode = code.substr(dataTokenStartPos, endPos - dataTokenStartPos);


			String nameCode = ReplaceAll(dataCode, HLSL::Token::RWTexture2D, TT(""));
			nameCode = ReplaceAll(nameCode, TT(" "), TT(""));

			u64 arraySize = 1;

			u64 arrayStartPos = dataCode.find(TT("["));
			if (arrayStartPos != String::npos)
			{

				nameCode = nameCode.substr(0, nameCode.find(TT("[")));



				arrayStartPos += 1;
				u64 arrayEndPos = dataCode.find(TT("]"), arrayStartPos);


				String arraySizeCode = dataCode.substr(arrayStartPos, arrayEndPos - arrayStartPos);
				arraySizeCode = ReplaceAll(arraySizeCode, TT(" "), TT(""));

				arraySize = _wtoi64(arraySizeCode.c_str());
			}

			if (RegisterRWTextureData(nameCode) == false)
			{
				if (result != nullptr)
				{
					*result = false;
				}
				return String::npos;
			}

			auto textureData = RWTextureDataMap[nameCode].get();
			textureData->Name = nameCode;
			textureData->RootParm = RootParamOffset++;
			textureData->RegisterNum = (u32)TextureRegisterNumberOffset; TextureRegisterNumberOffset += arraySize;
			textureData->RegisterSpace = ShaderDataForm::Texture2DStartSpace;
			textureData->ElementType = HLSL::EHLSLElement::RWTexture;
			textureData->Type = HLSL::EHLSLTextureType::_2D;
			textureData->TextureCount = arraySize;
			RootParamMap[textureData->RootParm] = textureData;


			code.insert(endPos,
				TT(" : register(u") + std::to_wstring(RWTextureDataMap[nameCode]->RegisterNum) +
				TT(", space") + std::to_wstring(RWTextureDataMap[nameCode]->RegisterSpace) + TT(")"));
			startPos = endPos + 1;
		}
		else
		{
			return String::npos;
		}
		return startPos;
	}
	void ShaderDataForm::ExtractCBufferName(const String& code, u64 pos, String* out_value)
	{
		u64 startPos = pos + wcslen(HLSL::Token::CBuffer);
		u64 endPos = code.find_first_of(TT("{"), startPos);
		String cbName = code.substr(startPos, endPos - startPos);
		cbName = ReplaceAll(cbName, TT("\n"), TT(""));
		cbName = ReplaceAll(cbName, TT("\t"), TT(""));
		cbName = ReplaceAll(cbName, TT(" "), TT(""));

		if (out_value != nullptr)
		{
			*out_value = cbName;
		}

	}
	u64 ShaderDataForm::ExtractCBufferVar(const String& code, u64 pos, String* out_value)
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
	u64 ShaderDataForm::ExtractSamplerStateValue(const String& samplerStateDataCode, u64 startPos, String* out_key, String* out_value)
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
	D3D12_STATIC_SAMPLER_DESC ShaderDataForm::CreateSamplerStateDesc(const Dictionary<String, String>& samplerDataMap)
	{
		D3D12_STATIC_SAMPLER_DESC desc = {};

		//
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::Template) != samplerDataMap.end())
		{
			CreateSamplerStateByTemplate(StringToSamplerStateTemplate(samplerDataMap.at(HLSL::Token::SamplerStateElement::Template)), &desc);
		}
		else
		{
			desc = CD3DX12_STATIC_SAMPLER_DESC(0);
		}

		String Min = HLSL::Token::SamplerSatateFilter::Point;
		String Mag = HLSL::Token::SamplerSatateFilter::Point;
		String Mip = HLSL::Token::SamplerSatateFilter::Point;

		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::Min) != samplerDataMap.end())
		{
			Min = samplerDataMap.at(HLSL::Token::SamplerStateElement::Min);
		}
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::Mag) != samplerDataMap.end())
		{
			Mag = samplerDataMap.at(HLSL::Token::SamplerStateElement::Mag);
		}
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::Mip) != samplerDataMap.end())
		{
			Mip = samplerDataMap.at(HLSL::Token::SamplerStateElement::Mip);
		}
		desc.Filter = GetSamplerStateFilter(Min, Mag, Mip);


		String AddressU = HLSL::Token::SamplerSatateAddressMode::Wrap;
		String AddressV = HLSL::Token::SamplerSatateAddressMode::Wrap;
		String AddressW = HLSL::Token::SamplerSatateAddressMode::Wrap;
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::AddressU) != samplerDataMap.end())
		{
			AddressU = samplerDataMap.at(HLSL::Token::SamplerStateElement::AddressU);
		}
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::AddressV) != samplerDataMap.end())
		{
			AddressV = samplerDataMap.at(HLSL::Token::SamplerStateElement::AddressV);
		}
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::AddressW) != samplerDataMap.end())
		{
			AddressW = samplerDataMap.at(HLSL::Token::SamplerStateElement::AddressW);
		}
		desc.AddressU = GetTextureAddressMode(AddressU);
		desc.AddressV = GetTextureAddressMode(AddressV);
		desc.AddressW = GetTextureAddressMode(AddressW);


		String ComparisonFunc = HLSL::Token::SamplerStateComparisonFunc::LessEqual;
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::ComparisonFunc) != samplerDataMap.end())
		{
			ComparisonFunc = samplerDataMap.at(HLSL::Token::SamplerStateElement::ComparisonFunc);
		}
		desc.ComparisonFunc = GetComparisonFunc(ComparisonFunc);


		String BorderColor = HLSL::Token::SamplerStateBorderColor::OpaqueBlack;
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::BorderColor) != samplerDataMap.end())
		{
			BorderColor = samplerDataMap.at(HLSL::Token::SamplerStateElement::BorderColor);
		}
		desc.BorderColor = GetBorderColor(BorderColor);


		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::MinLOD) != samplerDataMap.end())
		{
			desc.MinLOD = (f32)_wtof(samplerDataMap.at(HLSL::Token::SamplerStateElement::MinLOD).c_str());
		}
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::MaxLOD) != samplerDataMap.end())
		{
			desc.MaxLOD = (f32)_wtof(samplerDataMap.at(HLSL::Token::SamplerStateElement::MaxLOD).c_str());
		}
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::MaxAnisotropy) != samplerDataMap.end())
		{
			desc.MaxAnisotropy = (f32)_wtoi(samplerDataMap.at(HLSL::Token::SamplerStateElement::MaxAnisotropy).c_str());
		}
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::MipLODBias) != samplerDataMap.end())
		{
			desc.MipLODBias = (f32)_wtof(samplerDataMap.at(HLSL::Token::SamplerStateElement::MipLODBias).c_str());
		}
		return desc;
	}

	void ShaderDataForm::CreateSamplerStateByTemplate(ESamplerStateTemplate _template, D3D12_STATIC_SAMPLER_DESC* out_desc)
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
	D3D12_FILTER ShaderDataForm::GetSamplerStateFilter(const String& Min, const String& Mag, const String& Mip)
	{
		enum
		{
			Point = 0,
			Linear = 1,
			Anisotropic = 2
		};
		i32 min = Point; i32 mag = Point; i32 mip = Point;


		if (Min == HLSL::Token::SamplerSatateFilter::Point) min = Point;
		else if (Min == HLSL::Token::SamplerSatateFilter::Linear) min = Linear;
		else if (Min == HLSL::Token::SamplerSatateFilter::Anisotropic) min = Anisotropic;

		if (Mag == HLSL::Token::SamplerSatateFilter::Point) mag = Point;
		else if (Mag == HLSL::Token::SamplerSatateFilter::Linear) mag = Linear;
		else if (Mag == HLSL::Token::SamplerSatateFilter::Anisotropic) mag = Anisotropic;

		if (Mip == HLSL::Token::SamplerSatateFilter::Point) mip = Point;
		else if (Mip == HLSL::Token::SamplerSatateFilter::Linear) mip = Linear;
		else if (Mip == HLSL::Token::SamplerSatateFilter::Anisotropic) mip = Anisotropic;





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
	D3D12_TEXTURE_ADDRESS_MODE ShaderDataForm::GetTextureAddressMode(const String& addressMode)
	{
		if (addressMode == HLSL::Token::SamplerSatateAddressMode::Wrap) return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		else if (addressMode == HLSL::Token::SamplerSatateAddressMode::Mirror) return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		else if (addressMode == HLSL::Token::SamplerSatateAddressMode::Clamp) return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		else if (addressMode == HLSL::Token::SamplerSatateAddressMode::Border) return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		else if (addressMode == HLSL::Token::SamplerSatateAddressMode::MirrorOnce) return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
		else return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	}
	D3D12_COMPARISON_FUNC ShaderDataForm::GetComparisonFunc(const String& comparisonFunc)
	{
		if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::Never) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
		else if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::Less) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
		else if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::Equal) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_EQUAL;
		else if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::LessEqual) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL;
		else if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::Greater) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER;
		else if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::NotEqual) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NOT_EQUAL;
		else if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::GreaterEqual) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		else if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::Always) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS;
		else return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL;
	}
	D3D12_STATIC_BORDER_COLOR ShaderDataForm::GetBorderColor(const String& borderColor)
	{
		// BorderColor = TransparentBlack, OpaqueBlack, OpaqueWhite
		if (borderColor == HLSL::Token::SamplerStateBorderColor::TransparentBlack) return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		else if (borderColor == HLSL::Token::SamplerStateBorderColor::OpaqueBlack) return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		else if (borderColor == HLSL::Token::SamplerStateBorderColor::OpaqueWhite) return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		else return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	}

	bool ShaderDataForm::RegisterStructuredBuffer(const String& name)
	{

		if (StructuredBufferDataMap.find(name) != StructuredBufferDataMap.end())
		{
			JG_CORE_ERROR("{0} StructuredBuffer Already Exists.", ws2s(name));
			return false;
		}
		StructuredBufferDataMap[name] = CreateUniquePtr<StructuredBufferData>();

		return true;
	}
	bool ShaderDataForm::RegisterRWStructuredBuffer(const String& name)
	{
		if (RWStructuredBufferDataMap.find(name) != RWStructuredBufferDataMap.end())
		{
			JG_CORE_ERROR("{0} RWStructuredBuffer Already Exists.", ws2s(name));
			return false;
		}
		RWStructuredBufferDataMap[name] = CreateUniquePtr<StructuredBufferData>();

		return true;
	}
	bool ShaderDataForm::RegisterCBuffer(const String& name)
	{

		if (CBufferDataMap.find(name) != CBufferDataMap.end())
		{
			JG_CORE_ERROR("{0} CBuffer Already Exists.", ws2s(name));
			return false;
		}

		CBufferDataMap[name] = CreateUniquePtr<CBufferData>();
		return true;
	}

	bool ShaderDataForm::RegisterTextureData(const String& name)
	{
		if (TextureDataMap.find(name) != TextureDataMap.end())
		{
			JG_CORE_ERROR("{0} TextureData Already Exists.", ws2s(name));
			return false;
		}

		TextureDataMap[name] = CreateUniquePtr<TextureData>();

		return true;
	}

	bool ShaderDataForm::RegisterRWTextureData(const String& name)
	{
		if (RWTextureDataMap.find(name) != RWTextureDataMap.end())
		{
			JG_CORE_ERROR("{0} RWTextureData Already Exists.", ws2s(name));
			return false;
		}

		RWTextureDataMap[name] = CreateUniquePtr<TextureData>();

		return true;
	}

	bool ShaderDataForm::RegisterCBufferVar(CBufferData* cBuffer, const String& varCode, u64& uploadDataSize)
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
		Data& uploadData = *(cBuffer->DataMap[nameCode]);
		uploadData.Type = StringToShaderDataType(typeCode);
		uploadData.DataSize = GetShaderDataTypeSize(uploadData.Type);
		uploadData.DataPos = uploadDataSize;
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

	bool ShaderDataForm::RegisterSamplerStateData(const String& name)
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