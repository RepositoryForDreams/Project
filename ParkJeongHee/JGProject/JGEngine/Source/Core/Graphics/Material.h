#pragma once
#include "JGCore.h"


namespace JG
{
	class IShader;
	class IMaterial
	{
	public:
		virtual bool SetFloat(const String& name, float value) = 0;
		virtual bool SetFloat2(const String& name, const JVector2& value) = 0;
		virtual bool SetFloat3(const String& name, const JVector3& value) = 0;
		virtual bool SetFloat4(const String& name, const JVector4& value) = 0;
		virtual bool SetInt(const String& name, i32 value) = 0;
		virtual bool SetInt2(const String& name, const JVector2Int& value) = 0;
		virtual bool SetInt3(const String& name, const JVector3Int& value) = 0;
		virtual bool SetInt4(const String& name, const JVector4Int& value) = 0;
		virtual bool SetUint(const String& name, u32 value) = 0;
		virtual bool SetUint2(const String& name, const JVector2Uint& value) = 0;
		virtual bool SetUint3(const String& name, const JVector3Uint& value) = 0;
		virtual bool SetUint4(const String& name, const JVector4Uint& value) = 0;
		virtual bool SetFloat4x4(const String& name, const JMatrix& value) = 0;
	protected:
		virtual void Init(SharedPtr<IShader> shader) = 0;
		virtual bool Bind() = 0;
	public:
		static SharedPtr<IMaterial> Create(SharedPtr<IShader> shader);
	};



	class IMaterialInstance
	{
	public:
		virtual bool SetFloat(const String& name, float value) = 0;
		virtual bool SetFloat2(const String& name, const JVector2& value) = 0;
		virtual bool SetFloat3(const String& name, const JVector3& value) = 0;
		virtual bool SetFloat4(const String& name, const JVector4& value) = 0;
		virtual bool SetInt(const String& name, i32 value) = 0;
		virtual bool SetInt2(const String& name, const JVector2Int& value) = 0;
		virtual bool SetInt3(const String& name, const JVector3Int& value) = 0;
		virtual bool SetInt4(const String& name, const JVector4Int& value) = 0;
		virtual bool SetUint(const String& name, u32 value) = 0;
		virtual bool SetUint2(const String& name, const JVector2Uint& value) = 0;
		virtual bool SetUint3(const String& name, const JVector3Uint& value) = 0;
		virtual bool SetUint4(const String& name, const JVector4Uint& value) = 0;
		virtual bool SetFloat4x4(const String& name, const JMatrix& value) = 0;
	protected:
		virtual void Init(SharedPtr<IMaterial> material) = 0;
		virtual bool Bind() = 0;
	public:
		static SharedPtr<IMaterialInstance> Create(SharedPtr<IMaterial> material);
	};
}