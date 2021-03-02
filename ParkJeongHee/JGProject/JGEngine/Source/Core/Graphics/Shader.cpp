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
		std::lock_guard<std::shared_mutex> lock(GetInstance().mMutex);

		auto iter = GetInstance().mShaders.find(shader->GetName());
		if (iter == GetInstance().mShaders.end())
		{
			GetInstance().mShaders.emplace(shader->GetName(), shader);
		}
	}
	SharedPtr<IShader> ShaderLibrary::Get(const String& name)
	{
		std::shared_lock<std::shared_mutex> lock(GetInstance().mMutex);
		auto iter = GetInstance().mShaders.find(name);
		if (iter != GetInstance().mShaders.end())
		{
			return iter->second;
		}
		return nullptr;
	}

}