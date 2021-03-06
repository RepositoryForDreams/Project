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

			case DirectX12ShaderData::EShaderElementType::Texture:
				JGASSERT("IS NOT IMPL");
				break;

			case DirectX12ShaderData::EShaderElementType::SamplerState:
				JGASSERT("IS NOT IMPL");
				break;
			}
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

		pos = 0;
		while (pos != String::npos)
		{
			pos = AnalysisStructuredBuffer(code, pos, &result);
		}


		// Texture
		pos = 0;



		// StructuredBuffer

		// Sampler는 이미 저

		return result;
	}
	void DirectX12ShaderData::Reset()
	{
		RootParamOffset = 0; 
		SpaceOffset = 0;
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


		// 이름
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

				u64 startPos = (pos == 0) ? 0 : dataCode.find_first_of(TT("\n"), pos) + 1;
				if (startPos != String::npos)
				{
					u64 endPos = dataCode.find_first_of(TT(";"), startPos);
					if (endPos != String::npos)
					{
						String varCode = dataCode.substr(startPos, endPos - startPos + 1);
						varCode = ReplaceAll(varCode, TT("\n"), TT(""));
						varCode = ReplaceAll(varCode, TT("\t"), TT(""));
						if (RegisterCBufferVar(cBuffer, varCode, uploadDataSize) == false)
						{
							if (result != nullptr)
							{
								*result = false;
							}
							return String::npos;
						}

						pos = endPos + 1;
						continue;
					}
				}
				break;
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
		cBuffer->RegisterNum = CBufferDataMap.size() - 1;
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
			ReplaceAll(typeCode, TT(" "), TT(""));



			String nameCode = dataCode.substr(dataTypeEndPos + 1, dataCode.length() - dataTypeEndPos - 1);
			ReplaceAll(nameCode, TT(" "), TT(""));

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
			structuredBufferData->RegisterSpace = DirectX12ShaderData::StructuredBufferStartSpace + SpaceOffset++;
			structuredBufferData->ShaderElementType = EShaderElementType::StructuredBuffer;
			structuredBufferData->ElementDataSize   = GetShaderDataTypeSize(structuredBufferData->Type);



			
			code.insert(endPos, TT(" : register(t0, space") + std::to_wstring(structuredBufferData->RegisterSpace) + TT(")"));
			startPos = endPos + 1;
		}
		else
		{
			return String::npos;
		}
		return startPos;
	}
	u64 DirectX12ShaderData::AnalysisTexture(const String& code, u64 pos, bool* result)
	{
		return String::npos;
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



}

