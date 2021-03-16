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


	class DirectX12Shader;
	class IShader;
	class ITexture;
	class ShaderData
	{
	public:
		Dictionary<String, List<byte>>				  ByteDatas;
		Dictionary<String, List<SharedPtr<ITexture>>> TextureDatas;
		WeakPtr<IShader>					          OwnerShader;

	public:
		ShaderData(SharedPtr<IShader> shader);
	public:
		bool SetFloat(const String& name, float value) ;
		bool SetFloat2(const String& name, const JVector2& value) ;
		bool SetFloat3(const String& name, const JVector3& value) ;
		bool SetFloat4(const String& name, const JVector4& value) ;
		bool SetInt(const String& name, i32 value) ;
		bool SetInt2(const String& name, const JVector2Int& value) ;
		bool SetInt3(const String& name, const JVector3Int& value) ;
		bool SetInt4(const String& name, const JVector4Int& value) ;
		bool SetUint(const String& name, u32 value) ;
		bool SetUint2(const String& name, const JVector2Uint& value) ;
		bool SetUint3(const String& name, const JVector3Uint& value) ;
		bool SetUint4(const String& name, const JVector4Uint& value) ;
		bool SetFloat4x4(const String& name, const JMatrix& value) ;
		bool SetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture> texture) ;

		bool SetFloatArray(const String& name, const List<float>& value) ;
		bool SetFloat2Array(const String& name, const List<JVector2>& value) ;
		bool SetFloat3Array(const String& name, const List<JVector3>& value) ;
		bool SetFloat4Array(const String& name, const List<JVector4>& value) ;
		bool SetIntArray(const String& name, const List<i32>& value) ;
		bool SetInt2Array(const String& name, const List<JVector2Int>& value) ;
		bool SetInt3Array(const String& name, const List<JVector3Int>& value) ;
		bool SetInt4Array(const String& name, const List<JVector4Int>& value) ;
		bool SetUintArray(const String& name, const List<u32>& value) ;
		bool SetUint2Array(const String& name, const List<JVector2Uint>& value) ;
		bool SetUint3Array(const String& name, const List<JVector3Uint>& value) ;
		bool SetUint4Array(const String& name, const List<JVector4Uint>& value) ;
		bool SetFloat4x4Array(const String& name, const List<JMatrix>& value) ;
		bool SetStructDataArray(const String& name, void* datas, u64 elementCount, u64 elementSize) ;


		bool GetFloat(const String& name, float* out_value) ;
		bool GetFloat2(const String& name, JVector2* out_value) ;
		bool GetFloat3(const String& name, JVector3* out_value) ;
		bool GetFloat4(const String& name, JVector4* out_value) ;
		bool GetInt(const String& name, i32* out_value) ;
		bool GetInt2(const String& name, JVector2Int* out_value) ;
		bool GetInt3(const String& name, JVector3Int* out_value) ;
		bool GetInt4(const String& name, JVector4Int* out_value) ;
		bool GetUint(const String& name, u32* out_value) ;
		bool GetUint2(const String& name, JVector2Uint* out_value) ;
		bool GetUint3(const String& name, JVector3Uint* out_value) ;
		bool GetUint4(const String& name, JVector4Uint* out_value) ;
		bool GetFloat4x4(const String& name, JMatrix* outValue) ;
		bool GetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture>* out_value) ;

		bool GetFloatArray(const String& name, List<float>* out_value) ;
		bool GetFloat2Array(const String& name, List<JVector2>* out_value) ;
		bool GetFloat3Array(const String& name, List<JVector3>* out_value) ;
		bool GetFloat4Array(const String& name, List<JVector4>* out_value) ;
		bool GetIntArray(const String& name, List<i32>* out_value) ;
		bool GetInt2Array(const String& name, List<JVector2Int>* out_value) ;
		bool GetInt3Array(const String& name, List<JVector3Int>* out_value) ;
		bool GetInt4Array(const String& name, List<JVector4Int>* out_value) ;
		bool GetUintArray(const String& name, List<u32>* out_value) ;
		bool GetUint2Array(const String& name, List<JVector2Uint>* out_value) ;
		bool GetUint3Array(const String& name, List<JVector3Uint>* out_value) ;
		bool GetUint4Array(const String& name, List<JVector4Uint>* out_value) ;
		bool GetFloat4x4Array(const String& name, List<JMatrix>* out_value) ;
	public:
		template<class T, EShaderDataType type>
		bool SetData(const String& name, const T* value)
		{
			auto data = GetAndCheckData(name, type);
			if (data == nullptr)
			{
				return false;
			}

			u64 dataSize = sizeof(T);
			u64 dataPos = data->DataPos;
			String& cbName = data->Owner->Name;
			memcpy(&ByteDatas[cbName][dataPos], value, dataSize);
			return true;
		}



		template<class T, EShaderDataType type>
		bool SetDataArray(const String& name, const List<T>& dataArray)
		{
			if (CheckDataArray(name, type) == false)
			{
				return false;
			}
			u64 btSize = sizeof(T) * dataArray.size();


			memcpy(&ByteDatas[name][0], dataArray.data(), btSize);
			return true;
		}

		template<class T, EShaderDataType type>
		bool GetData(const String& name, T* value)
		{
			if (value == nullptr)
			{
				return false;
			}
			auto data = GetAndCheckData(name, type);
			if (data == nullptr)
			{
				return false;
			}
			u64 dataSize   = sizeof(T);
			u64 dataPos    = data->DataPos;
			String& cbName = data->Owner->Name;
			memcpy(value, &ByteDatas[cbName][dataPos], dataSize);
			return true;
		}
		ShaderDataForm::Data* GetAndCheckData(const String& name, EShaderDataType checkType);
		bool CheckDataArray(const String& name, EShaderDataType checkType);
	};
}