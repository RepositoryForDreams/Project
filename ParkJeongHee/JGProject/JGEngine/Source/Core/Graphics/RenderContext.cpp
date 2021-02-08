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

		switch (api->GetAPI())
		{
		case EGraphicsAPI::DirectX12:
		{
			auto result = CreateSharedPtr<DirectX12RenderContext>();
			if (!result->Init(settings))
			{
				JG_CORE_ERROR("Failed Create DirectX12RenderContext");
				return nullptr;
			}
			return result;
		}
		break;
		default:
			JG_CORE_ERROR("This API not supported Create IRenderContext");
		}
		return nullptr;

	}
}