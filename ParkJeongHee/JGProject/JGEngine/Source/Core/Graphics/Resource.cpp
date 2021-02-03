#include "pch.h"
#include "Resource.h"
#include "Application.h"


#include "Platform/Graphics/DirectX12/DirectX12Resource.h"


namespace JG
{
	SharedPtr<IVertexBuffer> IVertexBuffer::Create(String name, float* datas, u32 count)
	{
		JGASSERT_IF(datas != nullptr, TT("Data is Null"));
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		switch(api->GetAPI())
		{
		case EGraphicsAPI::DirectX12:
			{
				auto result = CreateSharedPtr<DirectX12VertexBuffer>();
				result->SetName(name);
				result->CreateBuffer(datas, count);
				return result;
			}
			break;
		default:
			JG_CORE_ERROR("This API not supported Create VertexBuffer");
		}
		return nullptr;
	}

	SharedPtr<IIndexBuffer> IIndexBuffer::Create(String name, u32* datas, u32 count)
	{
		JGASSERT_IF(datas == nullptr, TT("Data is Null"));
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		switch (api->GetAPI())
		{
		case EGraphicsAPI::DirectX12:
		{
			auto result = CreateSharedPtr<DirectX12IndexBuffer>();
			result->SetName(name);
			result->CreateBuffer(datas, count);
			return result;
		}
		break;
		default:
			JG_CORE_ERROR("This API not supported Create IndexBuffer");
		}
		return nullptr;
	}
}
