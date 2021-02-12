#include "pch.h"
#include "Renderer.h"
#include "Application.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
#include "Platform/Graphics/DirectX12/DirectX12Renderer.h"

namespace JG
{
	void Renderer3D::SubmitRenderContext(SharedPtr<IRenderContext> context)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		api->SubmitRenderContext(context);
	}




	void Renderer2D::Begin()
	{


	}

	SharedPtr<ITexture> Renderer2D::End()
	{
		return SharedPtr<ITexture>();

	}

	void Renderer2D::SubmitRenderContext(SharedPtr<IRenderContext> context)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		api->SubmitRenderContext(context);
	}
}