#include "pch.h"
#include "DirectX12Shader.h"
#include "Utill/PipelineState.h"
#include "Utill/RootSignature.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
namespace JG
{
	void DirectX12Shader::SetFloat(const String& name, float value)
	{
	}
	void DirectX12Shader::SetFloat2(const String& name, const JVector2& value)
	{
	}
	void DirectX12Shader::SetFloat3(const String& name, const JVector3& value)
	{
	}
	void DirectX12Shader::SetFloat4(const String& name, const JVector4& value)
	{
	}
	void DirectX12Shader::SetInt(const String& name, i32 value)
	{
	}
	void DirectX12Shader::SetInt2(const String& name, const JVector2Int& value)
	{
	}
	void DirectX12Shader::SetInt3(const String& name, const JVector3Int& value)
	{
	}
	void DirectX12Shader::SetInt4(const String& name, const JVector4Int& value)
	{
	}
	void DirectX12Shader::SetUint(const String& name, u32 value)
	{
	}
	void DirectX12Shader::SetUint2(const String& name, const JVector2Uint& value)
	{
	}
	void DirectX12Shader::SetUint3(const String& name, const JVector3Uint& value)
	{
	}
	void DirectX12Shader::SetUint4(const String& name, const JVector4Uint& value)
	{
	}
	void DirectX12Shader::SetFloat4x4(const String& name, const JMatrix& value)
	{
	}
	bool DirectX12Shader::Compile(const String& sourceCode, EShaderFlags flags, const String& error)
	{
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




		// RootSignature 정보
		// InputLayout   정보
		// dataToken   Param
		// MaterialParam Param
		// 
		



		// Shader Param

		Dictionary<String, UniquePtr<UploadData>> ShaderUploadDataMap;


		u64 dataTokenStartPos = sourceCode.find_first_of(ShaderCode::HLSL::CBDataToken);
		dataTokenStartPos     = sourceCode.find_first_of(TT("{"), dataTokenStartPos) + 1;
		if (dataTokenStartPos != String::npos)
		{
			u64 dataTokenEndPos = sourceCode.find_first_of(TT("}"), dataTokenStartPos);
			String dataCode   = sourceCode.substr(dataTokenStartPos, dataTokenEndPos - dataTokenStartPos);

			u64 pos = 0;
			while (pos != String::npos)
			{
				u64 startPos = dataCode.find_first_of(TT("\n"), pos) + 1;
				if (startPos != String::npos)
				{
					u64 endPos = dataCode.find_first_of(TT(";"), startPos);
					if (endPos != String::npos)
					{
						String varCode = dataCode.substr(startPos, endPos - startPos + 1);


						u64 varStartPos = varCode.find_first_not_of(TT(" "), varCode.find_first_not_of(TT("\t")));
						u64 varMidPos = varCode.find(TT(" "), varStartPos);
						u64 varEndPos = varCode.find(TT(";"), varMidPos) - 1;





						String typeCode = varCode.substr(varStartPos, varMidPos - varStartPos);
						String nameCode = varCode.substr(varMidPos + 1, varEndPos - varMidPos);

						UploadData upload;
						upload.Name = nameCode;
						//upload.Type = type;
						//upload.DataSize = GetShaderDataTypeSize(type);
						upload.DataPos = 0;
						pos = endPos + 1;
						continue;
					}
				}
				break;
			}
		

		}


		// RootSignature 정보
		// 
		// PipelineState
		// SetRenderTarget
		//





		return false;
	}

	bool DirectX12Shader::Bind()
	{
		// RootSignature 수정

		auto RootSig = DirectX12API::GetRootSignature();



		auto PSO = DirectX12API::GetGraphicsPipelineState();
		// PSO 수정
		PSO->BindShader(SharedPtr<DirectX12Shader>(this));



		return false;

	}
}

