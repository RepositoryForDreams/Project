﻿#pragma once
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
	class DirectX12ShaderData;





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
		UniquePtr<DirectX12ShaderData> mShaderData;
		ComPtr<ID3DBlob> mVSData;
		ComPtr<ID3DBlob> mDSData;
		ComPtr<ID3DBlob> mHSData;
		ComPtr<ID3DBlob> mGSData;
		ComPtr<ID3DBlob> mPSData;
		EShaderFlags     mFlags;
		bool mIsCompileSuccess = false;
	public:
		virtual bool Compile(const String& sourceCode, EShaderFlags flags, String* error) override;
		virtual bool Bind() override;
	public:
		virtual void  SetName(const String& name) override;
		virtual const String& GetName() const override;
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
		EShaderFlags GetFlags() const {
			return mFlags;
		}
		DirectX12ShaderData* GetShaderData() const {
			return mShaderData.get();
		}
	private:
		bool Compile(ComPtr<ID3DBlob>& blob, const String& sourceCode, const CompileConfig& config, String* error);
	};


	class DirectX12ShaderData
	{
	public:
		static const u64 Texture2DStartSpace   = 0;
		static const u64 TextureCubeStartSpace = 1;
		static const u64 StructuredBufferStartSpace = 2;
	public:
		class 	CBufferData;
		enum class EShaderElementType
		{
			None,
			CBuffer,
			StructuredBuffer,
			Texture,
			SamplerState,
		};
		enum class ETextureType
		{
			_1D,
			_2D,
			_3D,
			Cube,
		};
		class ShaderElement
		{
		public:
			String Name;
			u64 RootParm = 0;
			u32 RegisterNum   = 0;
			u32 RegisterSpace = 0;
			EShaderElementType ShaderElementType = EShaderElementType::None;
		public:
			virtual ~ShaderElement() = default;
		};
		class Data
		{
		public:
			EShaderDataType Type;
			u64 DataSize = 0;
			u64 DataPos  = 0;
			CBufferData* Owner = nullptr;
		};

		class TextureData : public ShaderElement
		{
		public:
			ETextureType Type = ETextureType::_2D;
			u64 TextureCount = 0;
		public:
			virtual ~TextureData() = default;
		};
		class SamplerStateData : public ShaderElement
		{
			D3D12_STATIC_SAMPLER_DESC Desc;
		public:
			virtual ~SamplerStateData() = default;
		};
		class CBufferData : public ShaderElement
		{
		public:
			Dictionary<String, UniquePtr<Data>> DataMap;
			u64 DataSize = 0;
		public:
			virtual ~CBufferData() = default;
		};
		class StructuredBufferData : public ShaderElement
		{
		public:
			EShaderDataType Type;
			u64 ElementDataSize = 0;
		};
	public:
		SortedDictionary<u64, ShaderElement*>			RootParamMap;
		Dictionary<String, UniquePtr<CBufferData>>		CBufferDataMap;
		Dictionary<String, UniquePtr<StructuredBufferData>> StructuredBufferDataMap;
		Dictionary<String, UniquePtr<TextureData>>		TextureDataMap;
		

		Dictionary<String, UniquePtr<SamplerStateData>> SamplerStateDataMap;
		Dictionary<String, Data*>		                CBufferVarMap;
	private:
		u64 RootParamOffset = 0;
		u64 TextureRegisterNumberOffset = 0;
		u64 TextureCubeRegisterNumberOffset = 0;
		u64 SpaceOffset     = 0;
	public:
		bool Set(String& code);
		void Reset();
	private:
		u64 AnalysisCBuffer(const String& code, u64 startPos, bool* result);
		u64 AnalysisStructuredBuffer(String& code, u64 startPos, bool* result);
		u64 AnalysisTexture2D(String& code, u64 startPos, bool* result);
		u64 AnalysisSamplerState(String& code, u64 startPos, bool* result);
	private:
		u64 ExtractSamplerStateValue(const String& samplerStateDataCode, u64 startPos, String* out_key, String* out_value);
	private:
		D3D12_STATIC_SAMPLER_DESC CreateSamplerStateDesc(const Dictionary<String, String>& samplerDataMap);
		D3D12_FILTER GetSamplerStateFilter(const String& Min, const String& Mag, const String& Mip);
		D3D12_TEXTURE_ADDRESS_MODE GetTextureAddressMode(const String& addressMode);
		D3D12_COMPARISON_FUNC GetComparisonFunc(const String& comparisonFunc);
		D3D12_STATIC_BORDER_COLOR GetBorderColor(const String& borderColor);
		void CreateSamplerStateByTemplate(ESamplerStateTemplate _template, D3D12_STATIC_SAMPLER_DESC* out_desc);

	private:
		bool RegisterStructuredBuffer(const String& name);
		bool RegisterCBuffer(const String& name);
		bool RegisterTextureData(const String& name);
		bool RegisterCBufferVar(CBufferData* cBuffer, const String& varCode, u64& uploadDataSize);


	};
}

