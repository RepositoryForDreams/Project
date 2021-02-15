#pragma once




#include "JGCore.h"
#include "Utill/DirectX12Helper.h"
#include "Graphics/Resource.h"



namespace JG
{
	class DirectX12VertexBuffer : public IVertexBuffer
	{
		u64   mElementSize  = 0;
		u64   mElementCount = 0;
		void* mCPUData  = nullptr;
	public:
		DirectX12VertexBuffer() = default;
		virtual ~DirectX12VertexBuffer();
	public:
		virtual bool  CreateBuffer(void* datas, u64 elementSize, u64 elementCount) override;
	protected:
		virtual void Bind() override;
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
	private:
		ComPtr<ID3D12Resource> mD3DResource;
		u32* mCPUData    = nullptr;
		u64  mIndexCount = 0;
	public:
		DirectX12IndexBuffer() = default;
		virtual ~DirectX12IndexBuffer();
	public:
		virtual bool  CreateBuffer(u32* datas, u64 count) override;
	protected:
		virtual void Bind() override;
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
	public:
		DirectX12Texture() = default;
		virtual ~DirectX12Texture();
	public:
		virtual TextureID GetTextureID() const override;
	protected:
		virtual void Bind() override;
	public:
		void Create(const String& name, const TextureInfo& info);
		void Reset();
	private:
		DirectX12Texture(const DirectX12Texture& texture) = delete;
		const DirectX12Texture& operator=(const DirectX12Texture& texture) = delete;
	};
}



