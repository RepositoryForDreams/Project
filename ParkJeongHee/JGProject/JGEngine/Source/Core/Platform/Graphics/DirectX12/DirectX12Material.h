#pragma once
#include "Graphics/Material.h"
#include "DirectX12Shader.h"


namespace JG
{
	class DirectX12Material : public IMaterial
	{
		//friend DirectX12API;
	private:
		Dictionary<String, List<byte>> mByteDatas;
		WeakPtr<IShader> mOwnerShader;
		String mName;
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

		virtual void  SetName(const String& name) override;
		virtual const String& GetName() const override;
	public:
		virtual bool Bind() override;
		virtual void Init(SharedPtr<IShader> shader) override;
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
			u64 dataPos = data->DataPos;
			String& cbName = data->Owner->Name;
			memcpy(&mByteDatas[cbName][dataPos], value, dataSize);
			return true;
		}
		DirectX12ShaderData::Data* GetAndCheckData(const String& name, EShaderDataType checkType);

	};

}