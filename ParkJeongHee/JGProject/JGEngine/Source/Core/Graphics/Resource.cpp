#include "pch.h"
#include "Resource.h"
#include "Application.h"


#include "Platform/Graphics/DirectX12/DirectX12Resource.h"


namespace JG
{
	SharedPtr<IVertexBuffer> IVertexBuffer::Create(String name, void* datas, u64 elementSize, u64 elementCount)
	{
		JGASSERT_IF(datas != nullptr, TT("Data is Null"));
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateVertexBuffer(name, datas, elementSize, elementCount);
	}

	SharedPtr<IIndexBuffer> IIndexBuffer::Create(String name, u32* datas, u64 count)
	{
		JGASSERT_IF(datas != nullptr, TT("Data is Null"));
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateIndexBuffer(name, datas, count);
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
