#pragma once
#include "JGCore.h"


namespace JG
{
	class IResource
	{
	private:
		String mName = TT("IResource");
	public:
		virtual void SetName(const String& name)
		{
			mName = name;
		}
		const String& GetName() const
		{
			return mName;
		}
	public:
		virtual void* GetUserData() const = 0;
	};

	class Buffer : public IResource { };


	class IVertexBuffer : public Buffer
	{
	public:
		IVertexBuffer() = default;
	protected:
		virtual bool CreateBuffer(void* datas, u64 elementSize, u64 elementCount) = 0;
	public:
		static SharedPtr<IVertexBuffer> Create(String name, void* datas, u64 elementSize,  u64 elementCount);
	private:
		IVertexBuffer(const IVertexBuffer&) = delete;
		const IVertexBuffer& operator=(const IVertexBuffer&) = delete;
	};

	class IIndexBuffer : public Buffer
	{
	public:
		IIndexBuffer() = default;
	protected:
		virtual bool CreateBuffer(u32* datas, u32 count) = 0;
	public:
		static SharedPtr<IIndexBuffer> Create(String name, u32* datas, u32 count);
	private:
		IIndexBuffer(const IIndexBuffer&) = delete;
		const IIndexBuffer& operator=(const IIndexBuffer&) = delete;
	};
	
	using TextureID = u64;
	class ITexture : public IResource
	{

		//
	public:
		virtual TextureID GetTextureID() const = 0;
	private:
		ITexture(const ITexture& texture) = delete;
		const ITexture& operator=(const ITexture& texture) = delete;
	};
}

