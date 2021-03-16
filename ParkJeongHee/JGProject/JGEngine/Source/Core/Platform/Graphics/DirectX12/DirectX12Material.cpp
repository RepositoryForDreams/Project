#include "pch.h"
#include "DirectX12Material.h"
#include "DirectX12API.h"
#include "Utill/CommandList.h"
#include "Utill/PipelineState.h"
#include "Utill/ShaderDataForm.h"
namespace JG
{
	bool DirectX12Material::SetFloat(const String& name, float value)
	{
		return mShaderData->SetFloat(name, value);
	}
	bool DirectX12Material::SetFloat2(const String& name, const JVector2& value)
	{
		return  mShaderData->SetFloat2(name, value);
	}
	bool DirectX12Material::SetFloat3(const String& name, const JVector3& value)
	{
		return  mShaderData->SetFloat3(name, value);
	}
	bool DirectX12Material::SetFloat4(const String& name, const JVector4& value)
	{
		return  mShaderData->SetFloat4(name, value);
	}
	bool DirectX12Material::SetInt(const String& name, i32 value)
	{
		return mShaderData->SetInt(name, value);
	}
	bool DirectX12Material::SetInt2(const String& name, const JVector2Int& value)
	{
		return mShaderData->SetInt2(name, value);
	}
	bool DirectX12Material::SetInt3(const String& name, const JVector3Int& value)
	{
		return mShaderData->SetInt3(name, value);
	}
	bool DirectX12Material::SetInt4(const String& name, const JVector4Int& value)
	{
		return mShaderData->SetInt4(name, value);
	}
	bool DirectX12Material::SetUint(const String& name, u32 value)
	{
		return mShaderData->SetUint(name, value);
	}
	bool DirectX12Material::SetUint2(const String& name, const JVector2Uint& value)
	{
		return mShaderData->SetUint2(name, value);
	}
	bool DirectX12Material::SetUint3(const String& name, const JVector3Uint& value)
	{
		return mShaderData->SetUint3(name, value);
	}
	bool DirectX12Material::SetUint4(const String& name, const JVector4Uint& value)
	{
		return mShaderData->SetUint4(name, value);
	}
	bool DirectX12Material::SetFloat4x4(const String& name, const JMatrix& value)
	{
		return mShaderData->SetFloat4x4(name, value);
	}

	bool DirectX12Material::SetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture> texture)
	{
		return mShaderData->SetTexture(name, textureSlot, texture);
	}

	bool DirectX12Material::SetFloatArray(const String& name, const List<float>& value)
	{
		return mShaderData->SetFloatArray(name, value);
	}

	bool DirectX12Material::SetFloat2Array(const String& name, const List<JVector2>& value)
	{
		return mShaderData->SetFloat2Array(name, value);
	}

	bool DirectX12Material::SetFloat3Array(const String& name, const List<JVector3>& value)
	{
		return mShaderData->SetFloat3Array(name, value);
	}

	bool DirectX12Material::SetFloat4Array(const String& name, const List<JVector4>& value)
	{
		return mShaderData->SetFloat4Array(name, value);
	}

	bool DirectX12Material::SetIntArray(const String& name, const List<i32>& value)
	{
		return mShaderData->SetIntArray(name, value);
	}

	bool DirectX12Material::SetInt2Array(const String& name, const List<JVector2Int>& value)
	{
		return mShaderData->SetInt2Array(name, value);
	}

	bool DirectX12Material::SetInt3Array(const String& name, const List<JVector3Int>& value)
	{
		return mShaderData->SetInt3Array(name, value);
	}

	bool DirectX12Material::SetInt4Array(const String& name, const List<JVector4Int>& value)
	{
		return mShaderData->SetInt4Array(name, value);
	}

	bool DirectX12Material::SetUintArray(const String& name, const List<u32>& value)
	{
		return mShaderData->SetUintArray(name, value);
	}

	bool DirectX12Material::SetUint2Array(const String& name, const List<JVector2Uint>& value)
	{
		return mShaderData->SetUint2Array(name, value);
	}

	bool DirectX12Material::SetUint3Array(const String& name, const List<JVector3Uint>& value)
	{
		return mShaderData->SetUint3Array(name, value);
	}

	bool DirectX12Material::SetUint4Array(const String& name, const List<JVector4Uint>& value)
	{
		return mShaderData->SetUint4Array(name, value);
	}

	bool DirectX12Material::SetFloat4x4Array(const String& name, const List<JMatrix>& value)
	{
		return mShaderData->SetFloat4x4Array(name, value);
	}

	bool DirectX12Material::SetStructDataArray(const String& name, void* datas, u64 elementCount, u64 elementSize)
	{
		JGASSERT("NOT Supported SetStructDataArray");
		return false;
	}



	bool DirectX12Material::GetFloat(const String& name, float* out_value)
	{
		return mShaderData->GetFloat(name, out_value);
	}

	bool DirectX12Material::GetFloat2(const String& name, JVector2* out_value)
	{
		return mShaderData->GetFloat2(name, out_value);
	}

	bool DirectX12Material::GetFloat3(const String& name, JVector3* out_value)
	{
		return mShaderData->GetFloat3(name, out_value);
	}

	bool DirectX12Material::GetFloat4(const String& name, JVector4* out_value)
	{
		return mShaderData->GetFloat4(name, out_value);
	}

	bool DirectX12Material::GetInt(const String& name, i32* out_value)
	{
		return mShaderData->GetInt(name, out_value);
	}

	bool DirectX12Material::GetInt2(const String& name, JVector2Int* out_value)
	{
		return mShaderData->GetInt2(name, out_value);
	}

	bool DirectX12Material::GetInt3(const String& name, JVector3Int* out_value)
	{
		return mShaderData->GetInt3(name, out_value);
	}

	bool DirectX12Material::GetInt4(const String& name, JVector4Int* out_value)
	{
		return mShaderData->GetInt4(name, out_value);
	}

	bool DirectX12Material::GetUint(const String& name, u32* out_value)
	{
		return mShaderData->GetUint(name, out_value);
	}

	bool DirectX12Material::GetUint2(const String& name, JVector2Uint* out_value)
	{
		return mShaderData->GetUint2(name, out_value);
	}

	bool DirectX12Material::GetUint3(const String& name, JVector3Uint* out_value)
	{
		return mShaderData->GetUint3(name, out_value);
	}

	bool DirectX12Material::GetUint4(const String& name, JVector4Uint* out_value)
	{
		return mShaderData->GetUint4(name, out_value);
	}

	bool DirectX12Material::GetFloat4x4(const String& name, JMatrix* out_value)
	{
		return mShaderData->GetFloat4x4(name, out_value);
	}

	bool DirectX12Material::GetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture>* out_value)
	{
		return mShaderData->GetTexture(name, textureSlot, out_value);
	}

	bool DirectX12Material::GetFloatArray(const String& name, List<float>* out_value)
	{
		return mShaderData->GetFloatArray(name, out_value);
	}

	bool DirectX12Material::GetFloat2Array(const String& name, List<JVector2>* out_value)
	{
		return mShaderData->GetFloat2Array(name, out_value);
	}

	bool DirectX12Material::GetFloat3Array(const String& name, List<JVector3>* out_value)
	{
		return mShaderData->GetFloat3Array(name, out_value);
	}

	bool DirectX12Material::GetFloat4Array(const String& name, List<JVector4>* out_value)
	{
		return mShaderData->GetFloat4Array(name, out_value);
	}

	bool DirectX12Material::GetIntArray(const String& name, List<i32>* out_value)
	{
		return mShaderData->GetIntArray(name, out_value);
	}

	bool DirectX12Material::GetInt2Array(const String& name, List<JVector2Int>* out_value)
	{
		return mShaderData->GetInt2Array(name, out_value);
	}

	bool DirectX12Material::GetInt3Array(const String& name, List<JVector3Int>* out_value)
	{
		return mShaderData->GetInt3Array(name, out_value);
	}

	bool DirectX12Material::GetInt4Array(const String& name, List<JVector4Int>* out_value)
	{
		return mShaderData->GetInt4Array(name, out_value);
	}

	bool DirectX12Material::GetUintArray(const String& name, List<u32>* out_value)
	{
		return mShaderData->GetUintArray(name, out_value);
	}

	bool DirectX12Material::GetUint2Array(const String& name, List<JVector2Uint>* out_value)
	{
		return mShaderData->GetUint2Array(name, out_value);
	}

	bool DirectX12Material::GetUint3Array(const String& name, List<JVector3Uint>* out_value)
	{
		return mShaderData->GetUint3Array(name, out_value);
	}

	bool DirectX12Material::GetUint4Array(const String& name, List<JVector4Uint>* out_value)
	{
		return mShaderData->GetUint4Array(name, out_value);
	}

	bool DirectX12Material::GetFloat4x4Array(const String& name, List<JMatrix>* out_value)
	{
		return mShaderData->GetFloat4x4Array(name, out_value);
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
		auto commandList  = DirectX12API::GetGraphicsCommandList();
		auto loadedShader = mShaderData->OwnerShader.lock();
		auto dx12Shader   = static_cast<DirectX12Shader*>(loadedShader.get());

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
				auto& byteData   = mShaderData->ByteDatas[cBufferName];
				commandList->BindDynamicConstantBuffer(cBufferData->RootParm, byteData.data(), byteData.size());
			}
		
			for (auto& _pair : shaderDataForm->TextureDataMap)
			{
				auto textureData = _pair.second.get();
				auto& textureList = mShaderData->TextureDatas[_pair.first];
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

			// RWTexture
			for (auto& _pair : shaderDataForm->RWTextureDataMap)
			{
				auto textureData = _pair.second.get();
				auto& textureList = mShaderData->TextureDatas[_pair.first];
				u64 textureCount = textureList.size();
				List<D3D12_CPU_DESCRIPTOR_HANDLE> handles;
				for (u64 i = 0; i < textureCount; ++i)
				{
					if (textureList[i] != nullptr && textureList[i]->IsValid())
					{
						handles.push_back(static_cast<DirectX12Texture*>(textureList[i].get())->GetUAV());
					}
				}

				if (handles.empty() == false)
				{
					commandList->BindTextures(textureData->RootParm, handles);
				}
			}
			// structuredBuffer
			for (auto& _pair : shaderDataForm->StructuredBufferDataMap)
			{
				auto structuredBufferName = _pair.first;
				auto structuredBufferData = _pair.second.get();
				auto& byteData = mShaderData->ByteDatas[structuredBufferName];
				commandList->BindDynamicStructuredBuffer(
					structuredBufferData->RootParm, byteData.data(), structuredBufferData->ElementDataSize, byteData.size());
			}

			// RWStructuredBuffer
			for (auto& _pair : shaderDataForm->RWStructuredBufferDataMap)
			{
				auto structuredBufferName = _pair.first;
				auto structuredBufferData = _pair.second.get();
				auto& byteData = mShaderData->ByteDatas[structuredBufferName];
				commandList->BindDynamicStructuredBuffer(
					structuredBufferData->RootParm, byteData.data(), structuredBufferData->ElementDataSize, byteData.size());
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
		mShaderData = CreateUniquePtr<ShaderData>(shader);
	}
}