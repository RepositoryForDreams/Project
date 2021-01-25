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
	protected:
		virtual bool CreateBuffer(float* datas, u32 count) = 0;
	public:
		static SharedPtr<IVertexBuffer> Create(String name, float* datas, u32 count);
	};

	class IIndexBuffer : public Buffer
	{
	protected:
		virtual bool CreateBuffer(u32* datas, u32 count) = 0;
	public:
		static SharedPtr<IIndexBuffer> Create(String name, u32* datas, u32 count);
	};
	
	using TextureID = u64;
	class ITexture : public IResource
	{


	public:
		virtual TextureID GetTextureID() const = 0;
	};
}

