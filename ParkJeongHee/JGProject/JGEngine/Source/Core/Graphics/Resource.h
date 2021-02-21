#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"

namespace JG
{
	class IResource
	{
	private:
		String mName = TT("IResource");
	public:
		IResource() = default;
		virtual ~IResource() = default;
		virtual void SetName(const String& name)
		{
			mName = name;
		}
		const String& GetName() const
		{
			return mName;
		}
	public:
		virtual bool IsValid() const = 0;
	protected:
		virtual void Bind()    = 0;

	private:
		IResource(const IResource&) = delete;
		const IResource& operator=(const IResource&) = delete;
	};

	class Buffer : public IResource { };


	class IVertexBuffer : public Buffer
	{
	public:
		virtual ~IVertexBuffer() = default;
	protected:
		virtual bool CreateBuffer(void* datas, u64 elementSize, u64 elementCount) = 0;
	public:
		static SharedPtr<IVertexBuffer> Create(String name, void* datas, u64 elementSize,  u64 elementCount);
	};

	class IIndexBuffer : public Buffer
	{
	public:
		virtual ~IIndexBuffer() = default;
	protected:
		virtual bool CreateBuffer(u32* datas, u64 count) = 0;
	public:
		static SharedPtr<IIndexBuffer> Create(String name, u32* datas, u64 count);
	};
	

	
	
	class ITexture : public IResource
	{
	public:
		ITexture() = default;
		virtual ~ITexture() = default;
	public:
		virtual TextureID          GetTextureID()   const = 0;
		virtual const TextureInfo& GetTextureInfo() const = 0;
		virtual void  SetTextureInfo(const TextureInfo& info) = 0;
	private:
		ITexture(const ITexture& texture) = delete;
		const ITexture& operator=(const ITexture& texture) = delete;
	public:
		static SharedPtr<ITexture> Create(const String& name, const TextureInfo& info);
		static SharedPtr<ITexture> CreateFromFile(const String& path);
	};
}

