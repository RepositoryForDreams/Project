#include "pch.h"
#include "Renderer.h"
#include "Application.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
#include "Platform/Graphics/DirectX12/DirectX12Renderer.h"

namespace JG
{
	void Renderer::Begin()
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		api->Begin();
	}

	void Renderer::End()
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		api->End();
	}
	void Renderer::Flush()
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		api->Flush();
	}
}