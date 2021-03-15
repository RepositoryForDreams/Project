#include "pch.h"
#include "DirectX12Material.h"
#include "DirectX12API.h"
#include "Utill/CommandList.h"
#include "Utill/PipelineState.h"
namespace JG
{
	bool DirectX12Material::SetFloat(const String& name, float value)
	{
		return SetData<float, EShaderDataType::_float>(name, &value);
	}
	bool DirectX12Material::SetFloat2(const String& name, const JVector2& value)
	{
		return SetData<JVector2, EShaderDataType::_float2>(name, &value);
	}
	bool DirectX12Material::SetFloat3(const String& name, const JVector3& value)
	{
		return SetData<JVector3, EShaderDataType::_float3>(name, &value);
	}
	bool DirectX12Material::SetFloat4(const String& name, const JVector4& value)
	{
		return SetData<JVector4, EShaderDataType::_float4>(name, &value);
	}
	bool DirectX12Material::SetInt(const String& name, i32 value)
	{
		return SetData<i32, EShaderDataType::_int>(name, &value);
	}
	bool DirectX12Material::SetInt2(const String& name, const JVector2Int& value)
	{
		return SetData<JVector2Int, EShaderDataType::_int2>(name, &value);
	}
	bool DirectX12Material::SetInt3(const String& name, const JVector3Int& value)
	{
		return SetData<JVector3Int, EShaderDataType::_int3>(name, &value);
	}
	bool DirectX12Material::SetInt4(const String& name, const JVector4Int& value)
	{
		return SetData<JVector4Int, EShaderDataType::_int4>(name, &value);
	}
	bool DirectX12Material::SetUint(const String& name, u32 value)
	{
		return SetData<u32, EShaderDataType::_uint>(name, &value);
	}
	bool DirectX12Material::SetUint2(const String& name, const JVector2Uint& value)
	{
		return SetData<JVector2Uint, EShaderDataType::_uint2>(name, &value);
	}
	bool DirectX12Material::SetUint3(const String& name, const JVector3Uint& value)
	{
		return SetData<JVector3Uint, EShaderDataType::_uint3>(name, &value);
	}
	bool DirectX12Material::SetUint4(const String& name, const JVector4Uint& value)
	{
		return SetData<JVector4Uint, EShaderDataType::_uint4>(name, &value);
	}
	bool DirectX12Material::SetFloat4x4(const String& name, const JMatrix& value)
	{
		return SetData<JMatrix, EShaderDataType::_float4x4> (name, &value);
	}

	bool DirectX12Material::SetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture> texture)
	{
		if (mTextureDatas.find(name) == mTextureDatas.end())
		{
			return false;
		}

		auto& textureList = mTextureDatas[name];
		u64 textureCount = textureList.size();

		if (textureCount <= textureSlot)
		{
			return false;
		}

		textureList[textureSlot] = texture;
		return true;
	}



	bool DirectX12Material::SetFloatArray(const String& name, const List<float>& value)
	{
		return SetDataArray<float, EShaderDataType::_float>(name, value);
	}

	bool DirectX12Material::SetFloat2Array(const String& name, const List<JVector2>& value)
	{
		return SetDataArray<JVector2, EShaderDataType::_float2>(name, value);
	}

	bool DirectX12Material::SetFloat3Array(const String& name, const List<JVector3>& value)
	{
		return SetDataArray<JVector3, EShaderDataType::_float3>(name, value);
	}

	bool DirectX12Material::SetFloat4Array(const String& name, const List<JVector4>& value)
	{
		return SetDataArray<JVector4, EShaderDataType::_float4>(name, value);
	}

	bool DirectX12Material::SetIntArray(const String& name, const List<i32>& value)
	{
		return SetDataArray<i32, EShaderDataType::_int>(name, value);
	}

	bool DirectX12Material::SetInt2Array(const String& name, const List<JVector2Int>& value)
	{
		return SetDataArray<JVector2Int, EShaderDataType::_int2>(name, value);
	}

	bool DirectX12Material::SetInt3Array(const String& name, const List<JVector3Int>& value)
	{
		return SetDataArray<JVector3Int, EShaderDataType::_int3>(name, value);
	}

	bool DirectX12Material::SetInt4Array(const String& name, const List<JVector4Int>& value)
	{
		return SetDataArray<JVector4Int, EShaderDataType::_int4>(name, value);
	}

	bool DirectX12Material::SetUintArray(const String& name, const List<u32>& value)
	{
		return SetDataArray<u32, EShaderDataType::_uint>(name, value);
	}

	bool DirectX12Material::SetUint2Array(const String& name, const List<JVector2Uint>& value)
	{
		return SetDataArray<JVector2Uint, EShaderDataType::_uint2>(name, value);
	}

	bool DirectX12Material::SetUint3Array(const String& name, const List<JVector3Uint>& value)
	{
		return SetDataArray<JVector3Uint, EShaderDataType::_uint3>(name, value);
	}

	bool DirectX12Material::SetUint4Array(const String& name, const List<JVector4Uint>& value)
	{
		return SetDataArray<JVector4Uint, EShaderDataType::_uint4>(name, value);
	}

	bool DirectX12Material::SetFloat4x4Array(const String& name, const List<JMatrix>& value)
	{
		return SetDataArray<JMatrix, EShaderDataType::_float4x4>(name, value);
	}

	bool DirectX12Material::SetStructDataArray(const String& name, void* datas, u64 elementCount, u64 elementSize)
	{
		JGASSERT("NOT Supported SetStructDataArray");
		return false;
	}



	bool DirectX12Material::GetFloat(const String& name, float* out_value)
	{
		return GetData<float, EShaderDataType::_float>(name, out_value);
	}

	bool DirectX12Material::GetFloat2(const String& name, JVector2* out_value)
	{
		return GetData<JVector2, EShaderDataType::_float2>(name, out_value);
	}

	bool DirectX12Material::GetFloat3(const String& name, JVector3* out_value)
	{
		return GetData<JVector3, EShaderDataType::_float3>(name, out_value);
	}

	bool DirectX12Material::GetFloat4(const String& name, JVector4* out_value)
	{
		return GetData<JVector4, EShaderDataType::_float4>(name, out_value);
	}

	bool DirectX12Material::GetInt(const String& name, i32* out_value)
	{
		return GetData<i32, EShaderDataType::_int>(name, out_value);
	}

	bool DirectX12Material::GetInt2(const String& name, JVector2Int* out_value)
	{
		return GetData<JVector2Int, EShaderDataType::_int2>(name, out_value);
	}

	bool DirectX12Material::GetInt3(const String& name, JVector3Int* out_value)
	{
		return GetData<JVector3Int, EShaderDataType::_int3>(name, out_value);
	}

	bool DirectX12Material::GetInt4(const String& name, JVector4Int* out_value)
	{
		return GetData<JVector4Int, EShaderDataType::_int4>(name, out_value);
	}

	bool DirectX12Material::GetUint(const String& name, u32* out_value)
	{
		return GetData<u32, EShaderDataType::_uint>(name, out_value);
	}

	bool DirectX12Material::GetUint2(const String& name, JVector2Uint* out_value)
	{
		return GetData<JVector2Uint, EShaderDataType::_uint2>(name, out_value);
	}

	bool DirectX12Material::GetUint3(const String& name, JVector3Uint* out_value)
	{
		return GetData<JVector3Uint, EShaderDataType::_uint3>(name, out_value);
	}

	bool DirectX12Material::GetUint4(const String& name, JVector4Uint* out_value)
	{
		return GetData<JVector4Uint, EShaderDataType::_uint4>(name, out_value);
	}

	bool DirectX12Material::GetFloat4x4(const String& name, JMatrix* outValue)
	{
		return GetData<JMatrix, EShaderDataType::_float4x4>(name, outValue);
	}

	bool DirectX12Material::GetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture>* out_value)
	{
		return false;
	}

	bool DirectX12Material::GetFloatArray(const String& name, List<float>* out_value)
	{
		return false;
	}

	bool DirectX12Material::GetFloat2Array(const String& name, List<JVector2>* out_value)
	{
		return false;
	}

	bool DirectX12Material::GetFloat3Array(const String& name, List<JVector3>* out_value)
	{
		return false;
	}

	bool DirectX12Material::GetFloat4Array(const String& name, List<JVector4>* out_value)
	{
		return false;
	}

	bool DirectX12Material::GetIntArray(const String& name, List<i32>* out_value)
	{
		return false;
	}

	bool DirectX12Material::GetInt2Array(const String& name, List<JVector2Int>* out_value)
	{
		return false;
	}

	bool DirectX12Material::GetInt3Array(const String& name, List<JVector3Int>* out_value)
	{
		return false;
	}

	bool DirectX12Material::GetInt4Array(const String& name, List<JVector4Int>* out_value)
	{
		return false;
	}

	bool DirectX12Material::GetUintArray(const String& name, List<u32>* out_value)
	{
		return false;
	}

	bool DirectX12Material::GetUint2Array(const String& name, List<JVector2Uint>* out_value)
	{
		return false;
	}

	bool DirectX12Material::GetUint3Array(const String& name, List<JVector3Uint>* out_value)
	{
		return false;
	}

	bool DirectX12Material::GetUint4Array(const String& name, List<JVector4Uint>* out_value)
	{
		return false;
	}

	bool DirectX12Material::GetFloat4x4Array(const String& name, List<JMatrix>* out_value)
	{
		return false;
	}

	void DirectX12Material::SetName(const String& name)
	{
		mName = name;
	}

	const String& DirectX12Material::GetName() const
	{
		return mName;
	}

	bool DirectX12Material::Bind()
	{
		auto commandList = DirectX12API::GetGraphicsCommandList();
		auto loadedShader = mOwnerShader.lock();
		auto dx12Shader = static_cast<DirectX12Shader*>(loadedShader.get());

		if (dx12Shader != nullptr)
		{
			if (dx12Shader->Bind() == false)
			{
				JG_CORE_ERROR("Failed Bind {0} Shader, in {1} Material", ws2s(dx12Shader->GetName()), ws2s(GetName()));
				return false;
			}


			auto shaderDataForm = dx12Shader->GetShaderDataForm();


			for (auto& _pair : shaderDataForm->CBufferDataMap)
			{
				auto cBufferName = _pair.first;
				auto cBufferData = _pair.second.get();
				auto& byteData   = mByteDatas[cBufferName];
				commandList->BindDynamicConstantBuffer(cBufferData->RootParm, byteData.data(), byteData.size());
			}
		
			for (auto& _pair : shaderDataForm->TextureDataMap)
			{
				auto textureData = _pair.second.get();
				auto& textureList = mTextureDatas[_pair.first];
				u64 textureCount = textureList.size();
				List<D3D12_CPU_DESCRIPTOR_HANDLE> handles;

				
				for (u64 i = 0; i < textureCount; ++i)
				{
					if (textureList[i] != nullptr && textureList[i]->IsValid())
					{
						handles.push_back(static_cast<DirectX12Texture*>(textureList[i].get())->GetSRV());
					}
				}

				if (handles.empty() == false)
				{
					commandList->BindTextures(textureData->RootParm, handles);
				}
		
			}

		}
		else
		{
			return false;
		}
		return true;
	}

	void DirectX12Material::Init(SharedPtr<IShader> shader)
	{
		mOwnerShader = shader;

		auto loadedShader = mOwnerShader.lock();
		auto dx12Shader = static_cast<DirectX12Shader*>(loadedShader.get());

		if (dx12Shader != nullptr)
		{
			auto shaderDataForm = dx12Shader->GetShaderDataForm();


			for (auto& _pair : shaderDataForm->CBufferDataMap)
			{
				mByteDatas[_pair.first].resize(_pair.second->DataSize, 0);
			}

			for (auto& _pair : shaderDataForm->StructuredBufferDataMap)
			{
				mByteDatas[_pair.first].clear();
			}

			for (auto& _pair : shaderDataForm->TextureDataMap)
			{
				mTextureDatas[_pair.first].resize(_pair.second->TextureCount, nullptr);
			}


		}
	}

	ShaderDataForm::Data* DirectX12Material::GetAndCheckData(const String& name, EShaderDataType checkType)
	{
		auto loadedShader = mOwnerShader.lock();
		auto dx12Shader = static_cast<DirectX12Shader*>(loadedShader.get());
		if (dx12Shader != nullptr)
		{
			auto shaderDataForm = dx12Shader->GetShaderDataForm();
			auto& CBufferVarMap = shaderDataForm->CBufferVarMap;

			auto iter = CBufferVarMap.find(name);
			if (iter == CBufferVarMap.end())
			{
				return nullptr;
			}
			auto data = iter->second;
			if (data->Type != checkType)
			{
				return nullptr;
			}
			return data;
		}
		else
		{
			return nullptr;
		}
	}
	bool DirectX12Material::CheckDataArray(const String& name, EShaderDataType checkType)
	{
		auto loadedShader = mOwnerShader.lock();
		auto dx12Shader = static_cast<DirectX12Shader*>(loadedShader.get());
		if (dx12Shader != nullptr)
		{
			auto shaderDataForm = dx12Shader->GetShaderDataForm();
			auto iter = shaderDataForm->StructuredBufferDataMap.find(name);
			if (iter == shaderDataForm->StructuredBufferDataMap.end())
			{
				return false;
			}

			auto data = iter->second.get();
			if (data->Type != checkType)
			{
				return false;
			}

			return true;
		}
		else
		{
			return false;
		}
		
	}
}