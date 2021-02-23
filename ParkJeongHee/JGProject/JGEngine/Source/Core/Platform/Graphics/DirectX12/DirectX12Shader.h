#pragma once
#include "Graphics/Shader.h"
#include "Utill/DirectX12Helper.h"

namespace JG
{
	/*
	ConstantBuffer
	StructedBuffer
	SamplerState 정보
	InputLayout  정보
	
	*/
	/*
	MaterialParam
	블라 블라 
	*/
	class GraphicsPipelineState;
	class RootSignature;
	class DirectX12Shader : public IShader
	{
	
		// 변수 이름, 변수 타입 , 변수 값
		// 저장하고있다가 상수 버퍼
	private:
		SharedPtr<GraphicsPipelineState> mPSO;
		SharedPtr<RootSignature>         mRootSig;
		ComPtr<ID3DBlob> mData;
		EShaderFlags     mFlags;
	public:
		virtual void SetFloat(const String& name, float value) override;
		virtual void SetFloat2(const String& name, const JVector2& value) override;
		virtual void SetFloat3(const String& name, const JVector3& value) override;
		virtual void SetFloat4(const String& name, const JVector4& value) override;
		virtual void SetInt(const String& name, i32 value) override;
		virtual void SetInt2(const String& name, const JVector2Int& value) override;
		virtual void SetInt3(const String& name, const JVector3Int& value) override;
		virtual void SetInt4(const String& name, const JVector4Int& value) override;
		virtual void SetUint(const String& name, u32 value) override;
		virtual void SetUint2(const String& name, const JVector2Uint& value) override;
		virtual void SetUint3(const String& name, const JVector3Uint& value) override;
		virtual void SetUint4(const String& name, const JVector4Uint& value) override;
		virtual void SetFloat4x4(const String& name, const JMatrix& value)   override;
	public:
		virtual bool Compile(const String& sourceCode, EShaderFlags flags, const String& error) override;
		virtual bool Bind();

	public:
		ID3DBlob* GetData() const {
			return mData.Get();
		}
		EShaderFlags GetFlags() const {
			return mFlags;
		}

	};
}

