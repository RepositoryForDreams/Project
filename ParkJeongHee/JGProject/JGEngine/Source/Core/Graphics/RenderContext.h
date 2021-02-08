#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"



namespace JG
{
	class Texture;
	struct RenderContextSettings
	{
		ptraddr Handle = 0;
		Color   ClearColor;
		ETextureFormat Format = ETextureFormat::None;
		u32 Width  = 0;
		u32 Height = 0;
		
		RenderContextSettings() = default;
		RenderContextSettings(
			ptraddr handle,
			Color clearColor,
			ETextureFormat format,
			u32 width,
			u32 height
		) : Handle(handle),
			Format(format),
			Width(width),
			Height(height),
			ClearColor(clearColor) {}

	};
	class IRenderContext
	{
	public:
		virtual bool Init(const RenderContextSettings& settings) = 0;
		virtual void SubmitTexture(const Texture& texture) = 0;
		virtual void Resize(u32 width, u32 height)		   = 0;
		virtual const RenderContextSettings& GetSettings() const = 0;
		virtual void Update() = 0;
		virtual bool Present() = 0;
	public:
		static SharedPtr<IRenderContext> Create(const RenderContextSettings& settings);
	};
}


