#include "pch.h"
#include "DirectX12Resource.h"
#include "DirectX12API.h"
#include "Utill/ResourceStateTracker.h"


namespace JG
{
	void DirectX12VertexBuffer::Reset()
	{
		if (mD3DResource == nullptr)
		{
			return;
		}
		ResourceStateTracker::UnRegisterResource(mD3DResource.Get());
		mD3DResource.Reset();
	}
	void* DirectX12VertexBuffer::GetUserData() const
	{
		return nullptr;
	}

	bool DirectX12VertexBuffer::CreateBuffer(void* datas, u64 elementSize, u64 elementCount)
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

	void DirectX12Texture::SetD3DResource(const String& name, ComPtr<ID3D12Resource> d3dResource, D3D12_RESOURCE_STATES initState)
	{
		Reset();
		mD3DResource = d3dResource;
		ResourceStateTracker::RegisterResource(name, mD3DResource.Get(), initState);
	}
	void DirectX12Texture::Reset()
	{
		if (mD3DResource == nullptr)
		{
			return;
		}
		ResourceStateTracker::UnRegisterResource(mD3DResource.Get());
		mD3DResource.Reset();
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
