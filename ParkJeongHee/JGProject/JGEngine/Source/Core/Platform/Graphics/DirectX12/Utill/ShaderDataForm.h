#pragma once
#include "JGCore.h"
#include "DirectX12Helper.h"
#include "Graphics/GraphicsDefine.h"

namespace JG
{
	class ShaderDataForm
	{
	public:
		static const u64 Texture2DStartSpace = 0;
		static const u64 TextureCubeStartSpace = 1;
		static const u64 StructuredBufferStartSpace = 2;
	public:
		class CBufferData;
		class ShaderElement
		{
		public:
			String Name;
			u64 RootParm = 0;
			u32 RegisterNum = 0;
			u32 RegisterSpace = 0;
			HLSL::EHLSLElement ElementType = HLSL::EHLSLElement::None;
		public:
			virtual ~ShaderElement() = default;
		};
		class Data
		{
		public:
			EShaderDataType Type;
			u64 DataSize = 0;
			u64 DataPos = 0;
			CBufferData* Owner = nullptr;
		};

		class TextureData : public ShaderElement
		{
		public:
			HLSL::EHLSLTextureType Type = HLSL::EHLSLTextureType::_2D;
			u64 TextureCount = 0;
		public:
			virtual ~TextureData() = default;
		};
		class SamplerStateData : public ShaderElement
		{
		public:
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
		Dictionary<String, UniquePtr<StructuredBufferData>> RWStructuredBufferDataMap;
		Dictionary<String, UniquePtr<TextureData>>		TextureDataMap;
		Dictionary<String, UniquePtr<TextureData>>		RWTextureDataMap;
		Dictionary<String, UniquePtr<SamplerStateData>> SamplerStateDataMap;
		Dictionary<String, Data*>		                CBufferVarMap;
	private:
		u64 RootParamOffset = 0;
		u64 TextureRegisterNumberOffset = 0;
		u64 TextureCubeRegisterNumberOffset = 0;
		u64 SamplerStateRegisterNumberOffset = 0;
		u64 SpaceOffset = 0;
	public:
		bool Set(String& code);
		void Reset();
	private:
		u64 AnalysisCBuffer(const String& code, u64 startPos, bool* result);
		u64 AnalysisStructuredBuffer(String& code, u64 startPos, bool* result);
		u64 AnalysisTexture2D(String& code, u64 startPos, bool* result);
		u64 AnalysisSamplerState(String& code, u64 startPos, bool* result);
		u64 AnalysisRWStructuredBuffer(String& code, u64 startPos, bool* result);
		u64 AnalysisRWTexture2D(String& code, u64 startPos, bool* result);
	private:
		void ExtractCBufferName(const String& code, u64 pos, String* out_value);
		u64 ExtractCBufferVar(const String& code, u64 pos, String* out_value);
		u64 ExtractSamplerStateValue(const String& samplerStateDataCode, u64 startPos, String* out_key, String* out_value);
	private:
		D3D12_STATIC_SAMPLER_DESC CreateSamplerStateDesc(const Dictionary<String, String>& samplerDataMap);
		void CreateSamplerStateByTemplate(ESamplerStateTemplate _template, D3D12_STATIC_SAMPLER_DESC* out_desc);


		D3D12_FILTER GetSamplerStateFilter(const String& Min, const String& Mag, const String& Mip);
		D3D12_TEXTURE_ADDRESS_MODE GetTextureAddressMode(const String& addressMode);
		D3D12_COMPARISON_FUNC GetComparisonFunc(const String& comparisonFunc);
		D3D12_STATIC_BORDER_COLOR GetBorderColor(const String& borderColor);
	private:
		bool RegisterStructuredBuffer(const String& name);
		bool RegisterRWStructuredBuffer(const String& name);
		bool RegisterCBuffer(const String& name);
		bool RegisterTextureData(const String& name);
		bool RegisterRWTextureData(const String& name);
		bool RegisterCBufferVar(CBufferData* cBuffer, const String& varCode, u64& uploadDataSize);
		bool RegisterSamplerStateData(const String& name);
	};
}