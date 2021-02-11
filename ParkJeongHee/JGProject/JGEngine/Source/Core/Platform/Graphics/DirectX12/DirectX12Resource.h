#pragma once




#include "JGCore.h"
#include "Utill/DirectX12Helper.h"
#include "Graphics/Resource.h"



namespace JG
{
	class DirectX12VertexBuffer : public IVertexBuffer
	{
	public:
		virtual void* GetUserData() const override;
	public:
		virtual bool  CreateBuffer(float* datas, u32 count) override;
	};


	class DirectX12IndexBuffer : public IIndexBuffer
	{
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
		DirectX12Texture();
	public:
		virtual void* GetUserData() const override;
	public:
		virtual TextureID GetTextureID() const override;


	private:
		DirectX12Texture(const DirectX12Texture& texture) = delete;
		const DirectX12Texture& operator=(const DirectX12Texture& texture) = delete;
	};
}



