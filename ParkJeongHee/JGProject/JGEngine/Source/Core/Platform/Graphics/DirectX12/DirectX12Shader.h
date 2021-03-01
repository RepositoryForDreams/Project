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
		virtual bool Bind();

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
	private:
		bool Compile(ComPtr<ID3DBlob> blob, const String& sourceCode, const CompileConfig& config, String* error);
	};


	class DirectX12ShaderData
	{
	
	public:
		class 	CBufferData;
		enum class EShaderElementType
		{
			None,
			CBuffer,
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
			u64 RootParm = 0;
			u32 RegisterNum   = 0;
			u32 RegisterSpace = 0;
			EShaderElementType ElementType = EShaderElementType::None;
		public:
			virtual ~ShaderElement() = default;
		};
		class Data
		{
		public:
			EShaderDataType Type;
			u64 DataSize = 0;
			u64 DataPos  = 0;
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
		class StructuredBufferData
		{
		public:

		};
	public:
		SortedDictionary<u64, ShaderElement*>			RootParamMap;
		Dictionary<String, UniquePtr<CBufferData>>		CBufferDataMap;
		Dictionary<String, UniquePtr<TextureData>>		TextureDataMap;
		Dictionary<String, UniquePtr<SamplerStateData>> SamplerStateDataMap;
		Dictionary<String, Data*>		                CBufferVarMap;
	private:
		u64 RootParamOffset = 0;
	public:
		bool Set(const String& code);
		void Reset();
	private:
		u64 AnalysisCBuffer(const String& code, u64 startPos, bool* result);
		u64 AnalysisTexture(const String& code, u64 startPos, bool* result);
	private:
		bool RegisterCBuffer(const String& name);
		bool RegisterCBufferVar(CBufferData* cBuffer, const String& varCode, u64& uploadDataSize);

	};
}

