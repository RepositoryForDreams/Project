#include "pch.h"
#include "DirectX12Resource.h"
#include "DirectX12API.h"
#include "Utill/ResourceStateTracker.h"
#include "Utill/CommandQueue.h"
#include "Utill/DescriptorAllocator.h"
#include "Utill/CommandList.h"
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
	bool DirectX12VertexBuffer::IsValid() const
	{
		return mCPUData != nullptr;
	}
	void DirectX12VertexBuffer::Bind()
	{
		auto commandList = DirectX12API::GetGraphicsCommandList();
		commandList->BindDynamicVertexBuffer(mCPUData, mElementSize, mElementCount);
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
	bool DirectX12IndexBuffer::IsValid() const
	{
		return mCPUData != nullptr;
	}

	void DirectX12IndexBuffer::Bind()
	{
		auto commandList = DirectX12API::GetGraphicsCommandList();
		commandList->BindDynamicIndexBuffer(mCPUData, mIndexCount);
		
	}



	DirectX12Texture::~DirectX12Texture()
	{
		
	}

	void DirectX12Texture::SetName(const String& name)
	{
		ITexture::SetName(name);
		if (mD3DResource != nullptr)
		{
			mD3DResource->SetName(name.c_str());
			ResourceStateTracker::SetResourceName(mD3DResource.Get(), name);
		}
	}

	TextureID DirectX12Texture::GetTextureID() const
	{
		// GetSRV
		return 0;
	}

	const TextureInfo& DirectX12Texture::GetTextureInfo() const
	{
		return mTextureInfo;
	}

	bool DirectX12Texture::IsValid() const
	{
		
		return mD3DResource != nullptr;;
	}

	void DirectX12Texture::Bind()
	{
		//  TODO
		// 고민
		
	}

	void DirectX12Texture::Create(const String& name, const TextureInfo& info)
	{
		SetName(name);
		SetTextureInfo(info);
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
	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Texture::GetRTV() const
	{
		if (IsValid() == false) return { 0 };

		SharedPtr<D3D12_RENDER_TARGET_VIEW_DESC> rtvDesc = nullptr;
		ETextureFlags textureFlags = mTextureInfo.Flags;

		// 렌더 타겟 텍스쳐가 아니라면 0을 뿜어냄
		if ((textureFlags & ETextureFlags::Allow_RenderTarget) == false)
		{
			return { 0 };
		}

		// 
		{
			// TODO
			// Flag 에따른 ReenderTarget 생성
		}

		u64 hash = std::hash<D3D12_RENDER_TARGET_VIEW_DESC>{}(*rtvDesc);

		D3D12_CPU_DESCRIPTOR_HANDLE handle = { 0 };
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(mRTVMutex);
			isFind = mRTVs.find(hash) != mRTVs.end();
			if (isFind == true)
			{
				handle = mRTVs[hash].CPU();
			}
		}
		if(isFind == false)
		{
			std::lock_guard<std::shared_mutex> lock(mRTVMutex);
			auto alloc = DirectX12API::RTVAllocate();
			DirectX12API::GetD3DDevice()->CreateRenderTargetView(Get(), rtvDesc.get(), alloc.CPU());
			mRTVs.emplace(hash, std::move(alloc));
			handle = mRTVs[hash].CPU();
		}

		return handle;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Texture::GetDSV() const
	{
		return D3D12_CPU_DESCRIPTOR_HANDLE();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Texture::GetSRV() const
	{
		return D3D12_CPU_DESCRIPTOR_HANDLE();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Texture::GetUAV() const
	{
		return D3D12_CPU_DESCRIPTOR_HANDLE();
	}
	void DirectX12Texture::SetTextureInfo(const TextureInfo& info)
	{
		Reset();

		mTextureInfo = info;
		auto flags = mTextureInfo.Flags;



		D3D12_RESOURCE_FLAGS d3dRscFlags = D3D12_RESOURCE_FLAG_NONE;

		if (flags & ETextureFlags::Allow_RenderTarget)
		{
			d3dRscFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}
		if (flags & ETextureFlags::Allow_DepthStencil)
		{
			d3dRscFlags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}


		D3D12_RESOURCE_DESC rscDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			ConvertDirectX12TextureFormat(mTextureInfo.Format), mTextureInfo.Width, mTextureInfo.Height,
			info.ArraySize, mTextureInfo.MipLevel, 1, 0,
			d3dRscFlags, D3D12_TEXTURE_LAYOUT_UNKNOWN, 0);

		
		DirectX12API::GetD3DDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			nullptr, 
			D3D12_RESOURCE_STATE_COMMON,
			nullptr, IID_PPV_ARGS(mD3DResource.GetAddressOf()));
		

		

		ResourceStateTracker::RegisterResource(GetName(), mD3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);
	}

}
