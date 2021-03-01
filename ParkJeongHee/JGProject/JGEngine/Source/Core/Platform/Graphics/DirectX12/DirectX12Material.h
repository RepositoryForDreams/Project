#pragma once
#include "Graphics/Material.h"



namespace JG
{
	class DirectX12Shader;
	class DirectX12Material : public IMaterial
	{
	private:
		WeakPtr<DirectX12Shader> mOwnerShader;
	public:
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
	protected:
		virtual bool Bind() override;
		virtual void Init(SharedPtr<IShader> shader) override;


		/*
			private:
		template<class T, EShaderDataType type>
		bool SetData(const String& name, const T* value)
		{
			auto data = GetAndCheckData(name, type);
			if (data == nullptr)
			{
				return false;
			}

			u64 dataSize = sizeof(T);
			u64 dataPos  = data->DataPos;

			memcpy(&data->Owner->ByteData[dataPos], value, dataSize);
			return true;
		}

		Data* GetAndCheckData(const String& name, EShaderDataType checkType);

			DirectX12ShaderData::Data* DirectX12ShaderData::GetAndCheckData(const String& name, EShaderDataType checkType)
	{
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
		*/
	};



	class DirectX12MaterialInstance : public IMaterialInstance
	{
	private:
		WeakPtr<DirectX12Material> mOwnerMaterial;
	public:
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

	protected:
		virtual bool Bind() override;
		virtual void Init(SharedPtr<IMaterial> material) override;
	};
}