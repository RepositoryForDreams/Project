#include "pch.h"
#include "Shader.h"
#include "Application.h"
#include "GraphicsAPI.h"

namespace JG
{
	SharedPtr<IShader> IShader::Create(const String& name, const String& sourceCode, EShaderFlags flags)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateShader(name, sourceCode, flags);
	}
	SharedPtr<IMaterialScript> IMaterialScript::Create(const String& name, const String& code)
	{
		auto script = CreateSharedPtr<MaterialScript>(name, code);
		return script;
	}
	void ShaderLibrary::RegisterShader(SharedPtr<IShader> shader)
	{
		if (shader == nullptr)
		{
			return;
		}
		std::lock_guard<std::shared_mutex> lock(mMutex);

		auto iter = mShaders.find(shader->GetName());
		if (iter == mShaders.end())
		{
			mShaders.emplace(shader->GetName(), shader);
		}
	}
	void ShaderLibrary::RegisterScirpt(SharedPtr<IMaterialScript> script)
	{
		if (script == nullptr)
		{
			return;
		}
		std::lock_guard<std::shared_mutex> lock(mMutex);

		auto iter = mMaterialScirpts.find(script->GetName());
		if (iter == mMaterialScirpts.end())
		{
			mMaterialScirpts.emplace(script->GetName(), script);
		}
	}
	SharedPtr<IShader> ShaderLibrary::GetShader(const String& name)
	{
		std::shared_lock<std::shared_mutex> lock(mMutex);
		auto iter = mShaders.find(name);
		if (iter != mShaders.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	SharedPtr<IMaterialScript> ShaderLibrary::GetScript(const String& name)
	{

		std::shared_lock<std::shared_mutex> lock(mMutex);
		auto iter = mMaterialScirpts.find(name);
		if (iter != mMaterialScirpts.end())
		{
			return iter->second;
		}
		return nullptr;
	}





}