#pragma once
#include "JGCore.h"


namespace JG
{
	class IResource
	{
	public:
		// 
		virtual const String& GetName() const = 0;
	};

	class Buffer : public IResource { };


	class IVertexBuffer : public Buffer
	{
	protected:
		virtual void CreateBuffer(float* datas, u32 count) = 0;
	public:
		static SharedPtr<IVertexBuffer> Create(float* datas, u32 count);
	};

	class IIndexBuffer : public Buffer
	{
	protected:
		virtual void CreateBuffer(u32* datas, u32 count) = 0;
	public:
		static SharedPtr<IIndexBuffer> Create(u32* datas, u32 count);
	};
	


	class ITexture : public IResource
	{
		
	};
}

