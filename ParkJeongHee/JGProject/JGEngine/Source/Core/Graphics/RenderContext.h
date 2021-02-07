#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"



namespace JG
{
	class Texture;
	struct RenderContextSettings
	{
		ptraddr Handle = 0;
		ETextureFormat Format = ETextureFormat::None;
		u32 Width  = 0;
		u32 Height = 0;


		RenderContextSettings(
			ptraddr handle,
			ETextureFormat format,
			u32 width,
			u32 height
		) : Handle(handle),
			Format(format),
			Width(width),
			Height(height) {}

	};
	class IRenderContext
	{
	public:
		virtual void SubmitTexture(const Texture& texture) = 0;
		virtual void Resize(u32 width, u32 height)		   = 0;
	protected:
		virtual bool Init(const RenderContextSettings& settings) = 0;
		virtual void Destroy()		                             = 0;
	public:
		static void Create(u64 handles);
	};
}


