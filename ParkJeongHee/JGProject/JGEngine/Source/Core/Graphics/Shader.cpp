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
		return SharedPtr<IMaterialScript>();
	}



}