#include "pch.h"
#include "Shader.h"
#include "Application.h"
#include "GraphicsAPI.h"

namespace JG
{
	SharedPtr<IShader> IShader::Create(const String& sourceCode, ShaderFlags flags, const String& error)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateShader(sourceCode, flags, error);
	}


	void ShaderLibrary::RegisterShader(const String& name, SharedPtr<IShader> shader)
	{
		if (shader == nullptr)
		{
			return;
		}
		auto& _mutex = GetInstance().mMutex;
		std::lock_guard<std::shared_mutex> lock(_mutex);

		auto iter = GetInstance().mShaders.find(name);
		if (iter == GetInstance().mShaders.end())
		{
			GetInstance().mShaders.emplace(name, shader);
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