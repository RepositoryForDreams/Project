﻿#include "pch.h"
#include "Renderer.h"
#include "Application.h"
#include "Graphics/Resource.h"
#include "GraphicsAPI.h"

namespace JG
{

	


	SharedPtr<ITexture> test;
	bool Renderer2D::Create()
	{
		TextureInfo info;
		info.ClearColor = Color::Blue();
		info.Width = 1920;
		info.Height = 1080;
		info.MipLevel = 1;
		info.Flags = ETextureFlags::Allow_RenderTarget;
		info.Format = ETextureFormat::R8G8B8A8_Unorm;

		test = ITexture::Create(TT("TestTexture"), info);
		return true;
	}
	void Renderer2D::Destroy()
	{
		test.reset();
		test = nullptr;
	}
	bool Renderer2D::Begin()
	{
		auto api = Application::GetInstance().GetGraphicsAPI();

		api->ClearRenderTarget({ test }, nullptr);


		return true;
	}

	SharedPtr<ITexture> Renderer2D::End()
	{


		return test;
	}


}