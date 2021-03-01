﻿#include "pch.h"
#include "Resource.h"
#include "Application.h"


#include "Platform/Graphics/DirectX12/DirectX12Resource.h"


namespace JG
{
	SharedPtr<IVertexBuffer> IVertexBuffer::Create(String name)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateVertexBuffer(name);
	}

	SharedPtr<IIndexBuffer> IIndexBuffer::Create(String name)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateIndexBuffer(name);
	}

	SharedPtr<ITexture> ITexture::Create(const String& name, const TextureInfo& info)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");


		return api->CreateTexture(name, info);
	}

	SharedPtr<ITexture> ITexture::CreateFromFile(const String& path)
	{
		return nullptr;
	}
}
