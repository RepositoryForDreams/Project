#include "pch.h"
#include "DirectX12Resource.h"
#include "DirectX12API.h"
#include "Utill/ResourceStateTracker.h"


namespace JG
{
	DirectX12VertexBuffer::~DirectX12VertexBuffer()
	{
		if (mCPUData)
		{
			free(mCPUData);
			mCPUData = nullptr;
		}
	}

	bool DirectX12VertexBuffer::CreateBuffer(void* datas, u64 elementSize, u64 elementCount)
	{
		if(mCPUData)
		{
			free(mCPUData);
			mCPUData = nullptr;
		}
		mElementSize  = elementSize;
		mElementCount = elementCount;

		
		u64 btSize = mElementSize * mElementCount;
		
		mCPUData = malloc(btSize);
		if(mCPUData == nullptr)
		{
			JG_CORE_ERROR("Failed Malloc CPUData in VertexBuffer : {0}", ws2s(GetName()));
			return false;
		}
		
		memcpy(mCPUData, datas, btSize);

		JG_CORE_INFO("Successed Create VertexBuffer =>  Name : {0}  ElementSize = {1}   ElementCount = {2}", ws2s(GetName()), mElementSize, mElementCount);
		return true;
	}

	void DirectX12VertexBuffer::Bind()
	{

		
		// 추후 구현

		
	}


	DirectX12IndexBuffer::~DirectX12IndexBuffer()
	{
		if (mCPUData)
		{
			free(mCPUData);
			mCPUData = nullptr;
		}
	}

	bool DirectX12IndexBuffer::CreateBuffer(u32* datas, u64 count)
	{
		if (mCPUData)
		{
			free(mCPUData);
			mCPUData = nullptr;
		}
		mIndexCount = count;

		u64 btSize = sizeof(u32) * count;

		mCPUData = (u32*)malloc(btSize);
		if (mCPUData == nullptr)
		{
			JG_CORE_ERROR("Failed Malloc CPUData in IndexBuffer : {0}", ws2s(GetName()));
			return false;
		}
		memcpy(mCPUData, datas, btSize);

		JG_CORE_INFO("Successed Create IndexBuffer =>  Name : {0}  IndexCount = {1}", ws2s(GetName()),mIndexCount);
		return true;
	}

	void DirectX12IndexBuffer::Bind()
	{

		
	}


	DirectX12Texture::~DirectX12Texture()
	{
		
	}

	TextureID DirectX12Texture::GetTextureID() const
	{
		return 0;
	}

	void DirectX12Texture::Bind()
	{

		
	}

	void DirectX12Texture::Create(const String& name, const TextureInfo& info)
	{
		Reset();

		// 생성
		ResourceStateTracker::RegisterResource(name, mD3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);
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
}
