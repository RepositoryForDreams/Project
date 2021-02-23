#pragma once

#include "JGCore.h"
#include "GraphicsDefine.h"

namespace JG
{
	/* Shader
	 * HLSL 컴 파일 하고 모시기 하고하는곳
	 * Compile 할때 코드 분석
	 *
	 * 
	 */


	

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
		virtual bool Compile(const String& sourceCode, EShaderFlags flags, const String& error) = 0;
		virtual bool Bind() = 0;
	public:
		static SharedPtr<IShader> Create(const String& sourceCode, EShaderFlags flags, const String& error);
	};


	class ShaderLibrary : GlobalSingleton<ShaderLibrary>
	{
	private:
		Dictionary<String, SharedPtr<IShader>> mShaders;
		std::shared_mutex mMutex;
	public:
		static void RegisterShader(const String& name, SharedPtr<IShader> shader);
		static SharedPtr<IShader> Get(const String& name);


	};



	
	
}


