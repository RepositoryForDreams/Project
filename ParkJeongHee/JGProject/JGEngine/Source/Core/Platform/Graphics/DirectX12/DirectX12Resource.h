#pragma once




#include "JGCore.h"
#include "Utill/DirectX12Helper.h"
#include "Graphics/Resource.h"



namespace JG
{
	class DirectX12VertexBuffer : public IVertexBuffer
	{
	private:
		ComPtr<ID3D12Resource> mD3DResource;
		D3D12_VERTEX_BUFFER_VIEW mView;
	public:
		void Reset();
	public:
		virtual void* GetUserData() const override;
	public:
		virtual bool  CreateBuffer(void* datas, u64 elementSize, u64 elementCount) override;
	};


	class DirectX12IndexBuffer : public IIndexBuffer
	{
	private:
		ComPtr<ID3D12Resource> mD3DResource;
	public:
		virtual void* GetUserData() const override;
	public:
		virtual bool  CreateBuffer(u32* datas, u32 count) override;
	};

	class DirectX12Texture : public ITexture
	{
	private:
		ComPtr<ID3D12Resource> mD3DResource;
	public:
		void SetD3DResource(const String& name, ComPtr<ID3D12Resource> d3dResource, D3D12_RESOURCE_STATES initState);
		void Reset();
	public:
		virtual void* GetUserData() const override;
	public:
		virtual TextureID GetTextureID() const override;


	private:
		DirectX12Texture(const DirectX12Texture& texture) = delete;
		const DirectX12Texture& operator=(const DirectX12Texture& texture) = delete;
	};
}



