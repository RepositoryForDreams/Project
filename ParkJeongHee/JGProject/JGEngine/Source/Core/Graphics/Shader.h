#pragma once

#include "JGCore.h"


namespace JG
{
	ENUM_FLAG(ShaderFlags)
	enum class ShaderFlags
	{
		None = 0x000,
		Allow_VertexShader   = 0x001,
		Allow_PixelShader	 = 0x002,
		Allow_GeometryShader = 0x004,
		Allow_HullShader     = 0x008,
		Allow_DomainShader   = 0x016,


	};
	/* Shader 자체
	
	*/
	class IShader
	{
	public:
		virtual void SetFloat(const String& name, float value) = 0;
		virtual void SetFloat2(const String& name, const JVector2& value) = 0;
		virtual void SetFloat3(const String& name, const JVector3& value) = 0;
		virtual void SetFloat4(const String& name, const JVector4& value) = 0;
		virtual void SetInt(const String& name, i32 value) = 0;
		virtual void SetInt2(const String& name, const JVector2Int& value) = 0;
		virtual void SetInt3(const String& name, const JVector3Int& value) = 0;
		virtual void SetInt4(const String& name, const JVector4Int& value) = 0;
		virtual void SetUint(const String& name, u32 value) = 0;
		virtual void SetUint2(const String& name, const JVector2Uint& value) = 0;
		virtual void SetUint3(const String& name, const JVector3Uint& value) = 0;
		virtual void SetUint4(const String& name, const JVector4Uint& value) = 0;
		virtual void SetFloat4x4(const String& name, const JMatrix& value)   = 0;
	protected:
		virtual bool Compile(const String& sourceCode, ShaderFlags flags, const String& error) = 0;
	public:
		static SharedPtr<IShader> Create(const String& sourceCode, ShaderFlags flags, const String& error);
	};


	class ShaderLibrary : GlobalSingleton<ShaderLibrary>
	{
	private:
		std::unordered_map<String, SharedPtr<IShader>> mShaders;
		std::shared_mutex mMutex;
	public:
		static void RegisterShader(const String& name, SharedPtr<IShader> shader);
		static SharedPtr<IShader> Get(const String& name);


	};



	
	
}


