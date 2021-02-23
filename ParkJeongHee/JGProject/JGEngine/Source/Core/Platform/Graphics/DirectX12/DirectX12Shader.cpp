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





	}
}

