#include "pch.h"
#include "DirectX12Resource.h"
#include "DirectX12API.h"



namespace JG
{
	void* DirectX12VertexBuffer::GetUserData() const
	{
		return nullptr;
	}

	bool DirectX12VertexBuffer::CreateBuffer(float* datas, u32 count)
	{

		return true;
	}

	void* DirectX12IndexBuffer::GetUserData() const
	{
		return nullptr;
	}

	bool DirectX12IndexBuffer::CreateBuffer(u32* datas, u32 count)
	{


		return true;
	}

	DirectX12Texture::DirectX12Texture()
	{
		// Resource 생성
		// Resource 등록



		
	}

	void* DirectX12Texture::GetUserData() const
	{
		return nullptr;
	}

	TextureID DirectX12Texture::GetTextureID() const
	{
		return 0;
	}
}
