#pragma once
#include "Graphics/Shader.h"
#include "Utill/DirectX12Helper.h"
#include "Utill/ShaderDataForm.h"
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
	class IComputeBuffer;
	class GraphicsPipelineState;
	class RootSignature;
	class ITexture;
	class ShaderDataForm;

	class DirectX12Shader : public IShader
	{
		struct CompileConfig
		{
			String Entry;
			String Target;
			CompileConfig(const String& entry, const String& target) : Entry(entry), Target(target) {}
		};
	private:
		String mName;
		UniquePtr<ShaderDataForm> mShaderDataForm;
		ComPtr<ID3DBlob> mVSData;
		ComPtr<ID3DBlob> mDSData;
		ComPtr<ID3DBlob> mHSData;
		ComPtr<ID3DBlob> mGSData;
		ComPtr<ID3DBlob> mPSData;
		ComPtr<ID3DBlob> mCSData;
		EShaderFlags     mFlags;
		bool mIsCompileSuccess = false;
	public:
		virtual bool Compile(const String& sourceCode, const List<SharedPtr<IMaterialScript>>& scriptList, EShaderFlags flags, String* error) override;
		virtual bool Bind() override;
	public:
		virtual void  SetName(const String& name) override;
		virtual const String& GetName() const override;
		virtual EShaderFlags GetFlags() const override
		{
			return mFlags;
		}
	private:
		bool GraphicsCompile(const String& code, String* error);
		void GraphicsBind(SharedPtr<RootSignature> RootSig);
		bool ComputeCompile(const String& code, String* error);
		void ComputeBind(SharedPtr<RootSignature> RootSig);

		void InsertScript(String& code, const List<SharedPtr<IMaterialScript>>& scriptList);
	public:
		ID3DBlob* GetVSData() const {
			return mVSData.Get();
		}
		ID3DBlob* GetDSData() const {
			return mDSData.Get();
		}
		ID3DBlob* GetHSData() const {
			return mHSData.Get();
		}
		ID3DBlob* GetGSData() const {
			return mGSData.Get();
		}
		ID3DBlob* GetPSData() const {
			return mPSData.Get();
		}
		ID3DBlob* GetCSData() const {
			return mCSData.Get();
		}
		ShaderDataForm* GetShaderDataForm() const {
			return mShaderDataForm.get();
		}
	private:
		bool Compile(ComPtr<ID3DBlob>& blob, const String& sourceCode, const CompileConfig& config, String* error);
	};





}

