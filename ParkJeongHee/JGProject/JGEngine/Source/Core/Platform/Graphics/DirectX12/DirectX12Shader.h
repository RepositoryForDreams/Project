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
	class IComputeBuffer;
	class GraphicsPipelineState;
	class RootSignature;
	class ITexture;
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
		virtual EShaderFlags GetFlags() const override
		{
			return mFlags;
		}
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
		DirectX12ShaderData* GetShaderData() const {
			return mShaderData.get();
		}
	private:
		bool Compile(ComPtr<ID3DBlob>& blob, const String& sourceCode, const CompileConfig& config, String* error);
	};


	class DirectX12ComputeShader : public IComputeShader
	{
		String mName;
		UniquePtr<DirectX12ShaderData> mShaderData;
		ComPtr<ID3DBlob> mCSData;
		bool mIsCompileSuccess = false;
	public:
		virtual bool Compile(const String& sourceCode, EShaderFlags flags, String* error) override;
		virtual bool Bind() override;
	public:
		virtual bool SetComputeBuffer(SharedPtr<IComputeBuffer> computeBuffer) override;
		virtual bool SetFloat(const String& name, float value) override;
		virtual bool SetFloat2(const String& name, const JVector2& value) override;
		virtual bool SetFloat3(const String& name, const JVector3& value) override;
		virtual bool SetFloat4(const String& name, const JVector4& value) override;
		virtual bool SetInt(const String& name, i32 value) override;
		virtual bool SetInt2(const String& name, const JVector2Int& value) override;
		virtual bool SetInt3(const String& name, const JVector3Int& value) override;
		virtual bool SetInt4(const String& name, const JVector4Int& value) override;
		virtual bool SetUint(const String& name, u32 value) override;
		virtual bool SetUint2(const String& name, const JVector2Uint& value) override;
		virtual bool SetUint3(const String& name, const JVector3Uint& value) override;
		virtual bool SetUint4(const String& name, const JVector4Uint& value) override;
		virtual bool SetFloat4x4(const String& name, const JMatrix& value) override;
		virtual bool SetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture> texture) override;

		virtual bool SetFloatArray(const String& name, const List<float>& value) override;
		virtual bool SetFloat2Array(const String& name, const List<JVector2>& value) override;
		virtual bool SetFloat3Array(const String& name, const List<JVector3>& value) override;
		virtual bool SetFloat4Array(const String& name, const List<JVector4>& value) override;
		virtual bool SetIntArray(const String& name, const List<i32>& value) override;
		virtual bool SetInt2Array(const String& name, const List<JVector2Int>& value) override;
		virtual bool SetInt3Array(const String& name, const List<JVector3Int>& value) override;
		virtual bool SetInt4Array(const String& name, const List<JVector4Int>& value) override;
		virtual bool SetUintArray(const String& name, const List<u32>& value) override;
		virtual bool SetUint2Array(const String& name, const List<JVector2Uint>& value) override;
		virtual bool SetUint3Array(const String& name, const List<JVector3Uint>& value) override;
		virtual bool SetUint4Array(const String& name, const List<JVector4Uint>& value) override;
		virtual bool SetFloat4x4Array(const String& name, const List<JMatrix>& value) override;
		virtual bool SetStructDataArray(const String& name, void* datas, u64 elementCount, u64 elementSize) override;
	public:
		virtual void Dispatch(u32 groupX, u32 groupY, u32 groupZ) override;
	public:
		virtual void  SetName(const String& name) override;
		virtual const String& GetName() const override;
	public:
		ID3DBlob* GetData() const {
			return mCSData.Get();
		}
	};


	class DirectX12ShaderData
	{
	public:
		static const u64 Texture2DStartSpace   = 0;
		static const u64 TextureCubeStartSpace = 1;
		static const u64 StructuredBufferStartSpace = 2;
	public:
		class 	CBufferData;
		class ShaderElement
		{
		public:
			String Name;
			u64 RootParm = 0;
			u32 RegisterNum   = 0;
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
			u64 DataPos  = 0;
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
		Dictionary<String, UniquePtr<TextureData>>		TextureDataMap;
		

		Dictionary<String, UniquePtr<SamplerStateData>> SamplerStateDataMap;
		Dictionary<String, Data*>		                CBufferVarMap;
	private:
		u64 RootParamOffset = 0;
		u64 TextureRegisterNumberOffset = 0;
		u64 TextureCubeRegisterNumberOffset = 0;
		u64 SamplerStateRegisterNumberOffset = 0;
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
		bool RegisterCBuffer(const String& name);
		bool RegisterTextureData(const String& name);
		bool RegisterCBufferVar(CBufferData* cBuffer, const String& varCode, u64& uploadDataSize);
		bool RegisterSamplerStateData(const String& name);
	};
}

