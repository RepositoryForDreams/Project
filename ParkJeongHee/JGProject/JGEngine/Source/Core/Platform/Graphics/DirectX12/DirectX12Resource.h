#pragma once




#include "JGCore.h"
#include "Utill/DirectX12Helper.h"
#include "Graphics/Resource.h"



namespace JG
{
	class DescriptorAllocation;
	class DirectX12VertexBuffer : public IVertexBuffer
	{
		friend class DirectX12Mesh;
	private:
		u64   mElementSize  = 0;
		u64   mElementCount = 0;
		void* mCPUData  = nullptr;
		EBufferLoadMethod mLoadMethod;
		ComPtr<ID3D12Resource>  mD3DResource;
	public:
		DirectX12VertexBuffer() = default;
		virtual ~DirectX12VertexBuffer();
	public:
		virtual bool  SetData(void* datas, u64 elementSize, u64 elementCount) override;
		virtual bool  IsValid() const override;
		virtual void SetBufferLoadMethod(EBufferLoadMethod type) override;
		virtual EBufferLoadMethod GetBufferLoadMethod() const override;
	protected:
		virtual void Bind();
		void Reset();
	public:
		void* GetData() const
		{
			return mCPUData;
		}
		u64 GetElementSize() const
		{
			return mElementSize;
		}
		u64 GetElementCount() const
		{
			return mElementCount;
		}
	};


	class DirectX12IndexBuffer : public IIndexBuffer
	{
		friend class DirectX12Mesh;
	private:
		u32* mCPUData    = nullptr;
		u64  mIndexCount = 0;
		EBufferLoadMethod mLoadMethod;
		ComPtr<ID3D12Resource>  mD3DResource;
	public:
		DirectX12IndexBuffer() = default;
		virtual ~DirectX12IndexBuffer();
	public:
		virtual bool SetData(u32* datas, u64 count) override;
		virtual bool IsValid() const override;

		virtual void SetBufferLoadMethod(EBufferLoadMethod method) override;
		virtual EBufferLoadMethod GetBufferLoadMethod() const override;
	protected:

		virtual void Bind();
		void Reset();
	public:
		u32* GetData() const
		{
			return mCPUData;
		}
		u64 GetIndexCount() const
		{
			return mIndexCount;
		}
	};

	class DirectX12Texture : public ITexture
	{
	private:
		ComPtr<ID3D12Resource> mD3DResource;
		TextureInfo mTextureInfo;

		mutable Dictionary<u64, DescriptorAllocation> mRTVs;
		mutable Dictionary<u64, DescriptorAllocation> mDSVs;
		mutable Dictionary<u64, DescriptorAllocation> mSRVs;
		mutable Dictionary<u64, DescriptorAllocation> mUAVs;

		mutable std::shared_mutex mRTVMutex;
		mutable std::shared_mutex mDSVMutex;
		mutable std::shared_mutex mSRVMutex;
		mutable std::shared_mutex mUAVMutex;

	public:
		DirectX12Texture() = default;
		virtual ~DirectX12Texture();
	public:
		virtual void SetName(const String& name) override;
		virtual TextureID          GetTextureID() const override;
		virtual const TextureInfo& GetTextureInfo() const override;
		virtual void SetTextureInfo(const TextureInfo& info) override;
		virtual bool IsValid() const override;
	protected:
		virtual void Bind() override;
	public:
		void Create(const String& name, const TextureInfo& info);
		void Reset();


		D3D12_CPU_DESCRIPTOR_HANDLE GetRTV() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetUAV() const;


		ID3D12Resource* Get() const {
			return mD3DResource.Get();
		}
		ID3D12Resource** GetAddress() {
			return mD3DResource.GetAddressOf();
		}
	private:
		SharedPtr<D3D12_RENDER_TARGET_VIEW_DESC> CreateRTVDesc(ETextureFlags flag) const;
		SharedPtr<D3D12_DEPTH_STENCIL_VIEW_DESC> CreateDSVDesc(ETextureFlags flag) const;
		SharedPtr<D3D12_SHADER_RESOURCE_VIEW_DESC>  CreateSRVDesc(ETextureFlags flag) const;
		SharedPtr<D3D12_UNORDERED_ACCESS_VIEW_DESC> CreateUAVDesc(ETextureFlags flag) const;
	};
}



