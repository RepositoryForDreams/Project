﻿#include "pch.h"
#include "DirectX12Resource.h"




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
}