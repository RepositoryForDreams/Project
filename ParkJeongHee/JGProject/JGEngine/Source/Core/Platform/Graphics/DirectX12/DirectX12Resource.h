#pragma once



#include "JGCore.h"
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


	

	
}



