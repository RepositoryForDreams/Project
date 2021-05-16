#pragma once

#include "JGCore.h"
#include "GraphicsDefine.h"
#include <shared_mutex>
namespace JG
{
	/* Shader

	// Global Shader 
	-> 무조건 포한하는 셰이더

	// Include Shader 
	-> 부분적으로 포함하는 셰이더

	// Material Shader
	-> 머터리얼 셰이더


	 */


	

	/* Shader 자체
	
	*/
	class IComputeBuffer;
	class ITexture;
	class IShader
	{
	protected:
		virtual bool Compile(const String& sourceCode, EShaderFlags flags, String* error) = 0;
		virtual bool Bind() = 0;
	public:
		virtual void SetName(const String& name) = 0;
		virtual const String& GetName() const    = 0;
		virtual EShaderFlags  GetFlags() const   = 0;
	public:
		static SharedPtr<IShader> Create(const String& name, const String& sourceCode, EShaderFlags flags);
	};
	class ShaderLibrary : public GlobalSingleton<ShaderLibrary>
	{
		friend class Application;
	private:
		Dictionary<String, SharedPtr<IShader>> mShaders;
		std::shared_mutex mMutex;
	public:
		static void RegisterShader(SharedPtr<IShader> shader);
		static SharedPtr<IShader> Get(const String& name);
	};



	
	
}


