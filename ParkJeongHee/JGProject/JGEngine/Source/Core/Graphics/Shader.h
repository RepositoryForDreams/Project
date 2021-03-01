﻿#pragma once

#include "JGCore.h"
#include "GraphicsDefine.h"
#include <shared_mutex>
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
	protected:
		virtual bool Compile(const String& sourceCode, EShaderFlags flags, String* error) = 0;
		virtual bool Bind() = 0;
	public:
		static SharedPtr<IShader> Create(const String& sourceCode, EShaderFlags flags);
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


