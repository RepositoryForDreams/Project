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


			auto shaderData = dx12Shader->GetShaderData();


			for (auto& _pair : shaderData->CBufferDataMap)
			{
				auto cBufferName = _pair.first;
				auto cBufferData = _pair.second.get();
				auto& byteData   = mByteDatas[cBufferName];
				commandList->BindDynamicConstantBuffer(cBufferData->RootParm, byteData.data(), byteData.size());
			}

			// TODO
			// Texture Bind
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
			auto shaderData = dx12Shader->GetShaderData();


			for (auto& _pair : shaderData->CBufferDataMap)
			{
				mByteDatas[_pair.first].resize(_pair.second->DataSize, 0);
			}

		}
	}

	DirectX12ShaderData::Data* DirectX12Material::GetAndCheckData(const String& name, EShaderDataType checkType)
	{
		auto loadedShader = mOwnerShader.lock();
		auto dx12Shader = static_cast<DirectX12Shader*>(loadedShader.get());
		if (dx12Shader != nullptr)
		{
			auto shaderData = dx12Shader->GetShaderData();
			auto& CBufferVarMap = shaderData->CBufferVarMap;

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
}