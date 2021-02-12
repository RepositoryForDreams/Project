#include "pch.h"
#include "RenderContext.h"
#include "Application.h"
#include "Graphics/GraphicsAPI.h"
#include "Platform/Graphics/DirectX12/DirectX12RenderContext.h"

namespace JG
{
	SharedPtr<IRenderContext> IRenderContext::Create(const RenderContextSettings& settings)
	{
		JGASSERT_IF(settings.Handle != 0, TT("Data is Null"));
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateRenderContext(settings);
	}
}