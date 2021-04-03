#include "pch.h"
#include "DevLayer.h"
#include "Application.h"
#include "Graphics/GraphicsAPI.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Camera.h"
#include "Graphics/Resource.h"





using namespace std;
namespace JG
{
	void DevLayer::OnAttach()
	{

	}
	void DevLayer::OnDetach()
	{
	
	}
	void DevLayer::Begin()
	{
		List<int> s;
		
		mCamera = Camera::Create(JVector2(1920, 1080), 60, 0.1f, 1000.0f, true);
		mCamera->SetLocation(JVector3(0, 0, -10));
		TextureInfo textureInfo;

		textureInfo.Width  = 1920;
		textureInfo.Height = 1080;
		textureInfo.ClearColor = Color::Green();
		textureInfo.Format = ETextureFormat::R8G8B8A8_Unorm;
		textureInfo.MipLevel = 1;
		textureInfo.Flags = ETextureFlags::Allow_RenderTarget;
		textureInfo.ArraySize = 1;
		auto texture = ITexture::Create(TT("DevLayer_Texture"), textureInfo);
		mCamera->SetTargetTexture(texture);
	}
	void DevLayer::Destroy()
	{

	}
	void DevLayer::OnEvent(IEvent& e)
	{

	}
	String DevLayer::GetLayerName()
	{
		return TT("DevLayer");
	}
}