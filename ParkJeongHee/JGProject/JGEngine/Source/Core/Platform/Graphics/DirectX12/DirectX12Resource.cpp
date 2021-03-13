#include "pch.h"
#include "DirectX12Resource.h"
#include "DirectX12API.h"
#include "Utill/ResourceStateTracker.h"
#include "Utill/CommandQueue.h"
#include "Utill/DescriptorAllocator.h"
#include "Utill/CommandList.h"
#include "Utill/PipelineState.h"
namespace JG
{
	DirectX12VertexBuffer::~DirectX12VertexBuffer()
	{
		Reset();
	}


	bool DirectX12VertexBuffer::SetData(void* datas, u64 elementSize, u64 elementCount)
	{
		u64 originBtSize = mElementSize * mElementCount;
		mElementSize = elementSize; mElementCount = elementCount;
		u64 btSize = mElementSize * mElementCount;

		// Create
		switch (mLoadMethod)
		{
		case EBufferLoadMethod::GPULoad:
		{
			Reset();
			auto d3dDevice = DirectX12API::GetD3DDevice();
			HRESULT hResult = d3dDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(btSize),
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				IID_PPV_ARGS(mD3DResource.GetAddressOf()));
			if (SUCCEEDED(hResult))
			{
				ResourceStateTracker::RegisterResource(GetName(), mD3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);

				auto commandList = DirectX12API::GetCopyCommandList();
				commandList->CopyBuffer(mD3DResource.Get(), datas, elementSize, elementCount);
			}
		}
		break;
		case EBufferLoadMethod::CPULoad:
			if (mD3DResource && originBtSize != btSize)
			{
				Reset();
			}
			if (mD3DResource == nullptr)
			{
				auto d3dDevice = DirectX12API::GetD3DDevice();
				HRESULT hResult = d3dDevice->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(btSize),
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(mD3DResource.GetAddressOf()));
				if (SUCCEEDED(hResult))
				{
					ResourceStateTracker::RegisterResource(GetName(), mD3DResource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ);
					mD3DResource->Map(0, nullptr, &mCPUData);
				}
			}
			memcpy(mCPUData, datas, btSize);
			break;
		}

	

		return true;
	}

	bool DirectX12VertexBuffer::IsValid() const
	{
		return mCPUData != nullptr;
	}
	EBufferLoadMethod DirectX12VertexBuffer::GetBufferLoadMethod() const
	{
		return mLoadMethod;
	}
	void DirectX12VertexBuffer::SetBufferLoadMethod(EBufferLoadMethod method)
	{
		 mLoadMethod = method;
	}
	void DirectX12VertexBuffer::Bind()
	{
		if (IsValid() == false)
		{
			return;
		}
		auto commandList = DirectX12API::GetGraphicsCommandList();


		D3D12_VERTEX_BUFFER_VIEW View = {};
		View.BufferLocation = mD3DResource->GetGPUVirtualAddress();
		View.SizeInBytes    = mElementSize * mElementCount;
		View.StrideInBytes  = mElementSize;
		commandList->BindVertexBuffer(View, false);
	}

	void DirectX12VertexBuffer::Reset()
	{
		switch (mLoadMethod)
		{
		case EBufferLoadMethod::CPULoad:
			if (mD3DResource)
			{
				mD3DResource->Unmap(0, nullptr);
				ResourceStateTracker::UnRegisterResource(mD3DResource.Get());
				mD3DResource.Reset(); mD3DResource = nullptr;
				mCPUData = nullptr;
			}
			break;
		case EBufferLoadMethod::GPULoad:
			if (mD3DResource)
			{
				ResourceStateTracker::UnRegisterResource(mD3DResource.Get());
				mD3DResource.Reset(); mD3DResource = nullptr;
			}
			break;
		}
	}




	DirectX12IndexBuffer::~DirectX12IndexBuffer()
	{
		Reset();
	}

	bool DirectX12IndexBuffer::SetData(u32* datas, u64 count)
	{
		u64 originBtSize = sizeof(u32) * mIndexCount;
		mIndexCount = count;
		u64 btSize = sizeof(u32) * mIndexCount;

		// Create
		switch (mLoadMethod)
		{
		case EBufferLoadMethod::GPULoad:
		{
			Reset();
			auto d3dDevice = DirectX12API::GetD3DDevice();
			HRESULT hResult = d3dDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(btSize),
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				IID_PPV_ARGS(mD3DResource.GetAddressOf()));
			if (SUCCEEDED(hResult))
			{
				ResourceStateTracker::RegisterResource(GetName(), mD3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);

				auto commandList = DirectX12API::GetCopyCommandList();
				commandList->CopyBuffer(mD3DResource.Get(), datas, sizeof(u32), mIndexCount);
			}
		}
		break;
		case EBufferLoadMethod::CPULoad:
			if (mD3DResource && originBtSize != btSize)
			{
				Reset();
			}
			if (mD3DResource == nullptr)
			{
				auto d3dDevice = DirectX12API::GetD3DDevice();
				HRESULT hResult = d3dDevice->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(btSize),
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(mD3DResource.GetAddressOf()));
				if (SUCCEEDED(hResult))
				{
					ResourceStateTracker::RegisterResource(GetName(), mD3DResource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ);
					mD3DResource->Map(0, nullptr, (void**)&mCPUData);
				}
			}
			memcpy(mCPUData, datas, btSize);
			break;
		}
		return true;
	}

	bool DirectX12IndexBuffer::IsValid() const
	{
		return mCPUData != nullptr;
	}
	EBufferLoadMethod DirectX12IndexBuffer::GetBufferLoadMethod() const
	{
		return mLoadMethod;
	}
	void DirectX12IndexBuffer::SetBufferLoadMethod(EBufferLoadMethod method)
	{
		mLoadMethod = method;
	}

	void DirectX12IndexBuffer::Bind()
	{
		if (IsValid() == false)
		{
			return;
		}
		auto commandList = DirectX12API::GetGraphicsCommandList();
		D3D12_INDEX_BUFFER_VIEW View;
		View.BufferLocation = mD3DResource->GetGPUVirtualAddress();
		View.Format = DXGI_FORMAT_R32_UINT;
		View.SizeInBytes = sizeof(u32) * mIndexCount;

		commandList->BindIndexBuffer(View);
	}

	void DirectX12IndexBuffer::Reset()
	{
		switch (mLoadMethod)
		{
		case EBufferLoadMethod::CPULoad:
			if (mD3DResource)
			{
				mD3DResource->Unmap(0, nullptr);
				ResourceStateTracker::UnRegisterResource(mD3DResource.Get());
				mD3DResource.Reset(); mD3DResource = nullptr;
				mCPUData = nullptr;
			}
			break;
		case EBufferLoadMethod::GPULoad:
			if (mD3DResource)
			{
				ResourceStateTracker::UnRegisterResource(mD3DResource.Get());
				mD3DResource.Reset(); mD3DResource = nullptr;
			}
			break;
		}
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
		return GetSRV().ptr;
	}

	const TextureInfo& DirectX12Texture::GetTextureInfo() const
	{
		return mTextureInfo;
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
		else if (flags & ETextureFlags::Allow_DepthStencil)
		{
			d3dRscFlags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}


		D3D12_RESOURCE_DESC rscDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			ConvertDXGIFormat(mTextureInfo.Format), mTextureInfo.Width, mTextureInfo.Height,
			info.ArraySize, mTextureInfo.MipLevel, 1, 0,
			d3dRscFlags, D3D12_TEXTURE_LAYOUT_UNKNOWN, 0);


		D3D12_CLEAR_VALUE clearValue;
		if (flags & ETextureFlags::Allow_RenderTarget)
		{
			clearValue.Color[0] = info.ClearColor.R;
			clearValue.Color[1] = info.ClearColor.G;
			clearValue.Color[2] = info.ClearColor.B;
			clearValue.Color[3] = info.ClearColor.A;
		}
		else if (flags & ETextureFlags::Allow_DepthStencil)
		{
			clearValue.DepthStencil.Depth = info.ClearDepth;
			clearValue.DepthStencil.Stencil = info.ClearStencil;
		}
		clearValue.Format = ConvertDXGIFormat(info.Format);

		DirectX12API::GetD3DDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&rscDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&clearValue, IID_PPV_ARGS(mD3DResource.GetAddressOf()));
		mD3DResource->SetName(GetName().c_str());
		ResourceStateTracker::RegisterResource(GetName(), mD3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);
	}
	bool DirectX12Texture::IsValid() const
	{
		
		return mD3DResource != nullptr;;
	}

	void DirectX12Texture::Bind()
	{
		//  TODO
		// 고민
		/*
		*	텍스쳐 바인딩

		
		*/
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
		// 렌더 타겟 텍스쳐가 아니라면 0을 뿜어냄
		if ((mTextureInfo.Flags & ETextureFlags::Allow_RenderTarget) == false)
		{
			JG_CORE_ERROR("{0} not supported RenderTarget because does not include  ETextureFlags::Allow_RenderTarget Flag", ws2s(GetName()));
			return { 0 };
		}

		SharedPtr<D3D12_RENDER_TARGET_VIEW_DESC> rtvDesc = CreateRTVDesc(mTextureInfo.Flags);


		u64 hash = 0;
		if (rtvDesc != nullptr)
		{
			hash = std::hash<D3D12_RENDER_TARGET_VIEW_DESC>{}(*rtvDesc);
		}

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
		if (IsValid() == false) return { 0 };
		if ((mTextureInfo.Flags & ETextureFlags::Allow_DepthStencil) == false)
		{
			JG_CORE_ERROR("{0} not supported DepthStencil because does not include  ETextureFlags::Allow_DepthStencil Flag", ws2s(GetName()));
			return { 0 };
		}

		SharedPtr<D3D12_DEPTH_STENCIL_VIEW_DESC> dsvDesc = CreateDSVDesc(mTextureInfo.Flags);

		u64 hash = 0;
		if (dsvDesc != nullptr)
		{
			hash = std::hash<D3D12_DEPTH_STENCIL_VIEW_DESC>{}(*dsvDesc);
		}


		D3D12_CPU_DESCRIPTOR_HANDLE handle = { 0 };
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(mDSVMutex);
			isFind = mDSVs.find(hash) != mDSVs.end();
			if (isFind == true)
			{
				handle = mDSVs[hash].CPU();
			}
		}
		if (isFind == false)
		{
			std::lock_guard<std::shared_mutex> lock(mDSVMutex);
			auto alloc = DirectX12API::DSVAllocate();
			DirectX12API::GetD3DDevice()->CreateDepthStencilView(Get(), dsvDesc.get(), alloc.CPU());
			mDSVs.emplace(hash, std::move(alloc));
			handle = mDSVs[hash].CPU();
		}

		return handle;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Texture::GetSRV() const
	{
		if (IsValid() == false) return { 0 };

		SharedPtr<D3D12_SHADER_RESOURCE_VIEW_DESC> srvDesc = CreateSRVDesc(mTextureInfo.Flags);

		u64 hash = 0;
		if (srvDesc != nullptr)
		{
			hash = std::hash<D3D12_SHADER_RESOURCE_VIEW_DESC>{}(*srvDesc);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle = { 0 };
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(mSRVMutex);
			isFind = mSRVs.find(hash) != mSRVs.end();
			if (isFind == true)
			{
				handle = mSRVs[hash].CPU();
			}
		}
		if (isFind == false)
		{
			std::lock_guard<std::shared_mutex> lock(mSRVMutex);
			auto alloc = DirectX12API::CSUAllocate();
			DirectX12API::GetD3DDevice()->CreateShaderResourceView(Get(), srvDesc.get(), alloc.CPU());
			mSRVs.emplace(hash, std::move(alloc));
			handle = mSRVs[hash].CPU();
		}

		return handle;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Texture::GetUAV() const
	{
		if (IsValid() == false) return { 0 };

		SharedPtr<D3D12_UNORDERED_ACCESS_VIEW_DESC> uavDesc = CreateUAVDesc(mTextureInfo.Flags);
		u64 hash = 0;
		if (uavDesc != nullptr)
		{
			hash = std::hash<D3D12_UNORDERED_ACCESS_VIEW_DESC>{}(*uavDesc);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle = { 0 };
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(mUAVMutex);
			isFind = mUAVs.find(hash) != mUAVs.end();
			if (isFind == true)
			{
				handle = mUAVs[hash].CPU();
			}
		}
		if (isFind == false)
		{
			std::lock_guard<std::shared_mutex> lock(mUAVMutex);
			auto alloc = DirectX12API::CSUAllocate();
			DirectX12API::GetD3DDevice()->CreateUnorderedAccessView(Get(), nullptr, uavDesc.get(), alloc.CPU());
			mUAVs.emplace(hash, std::move(alloc));
			handle = mUAVs[hash].CPU();
		}

		return handle;
	}

	SharedPtr<D3D12_RENDER_TARGET_VIEW_DESC> DirectX12Texture::CreateRTVDesc(ETextureFlags flag) const
	{
		return nullptr;
	}

	SharedPtr<D3D12_DEPTH_STENCIL_VIEW_DESC> DirectX12Texture::CreateDSVDesc(ETextureFlags flag) const
	{
		return nullptr;
	}

	SharedPtr<D3D12_SHADER_RESOURCE_VIEW_DESC> DirectX12Texture::CreateSRVDesc(ETextureFlags flag) const
	{
		return nullptr;
	}

	SharedPtr<D3D12_UNORDERED_ACCESS_VIEW_DESC> DirectX12Texture::CreateUAVDesc(ETextureFlags flag) const
	{
		return nullptr;
	}
	

}
