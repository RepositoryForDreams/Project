#include "DevLayer.h"
#include "Application.h"
#include "Platform/Window/Window.h"
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
	void DevLayer::Update()
	{

		if (Renderer2D::Begin(mCamera, mRenderTexture))
		{


			Renderer2D::DrawCall(
				JVector2(0,0), JVector2(200, 200)
			);

			Renderer2D::DrawCall(
				JVector2(400, 0), JVector2(200, 200)
			);

			Renderer2D::End();

		}



	}
	void DevLayer::LateUpdate()
	{

	}
	void DevLayer::Begin()
	{
		auto AppSize = Application::GetInstance().GetSize();
		FrameBufferInfo info = FrameBufferInfo(
			Application::GetInstance().GetWindow()->GetHandle(),
			Color::Red(),
			ETextureFormat::R8G8B8A8_Unorm,
			AppSize.x, AppSize.y);

		mFrameBuffer = IFrameBuffer::Create(info);



		mCamera = Camera::Create(JVector2(1920, 1080), 60, 0.1f, 1000.0f, true);
		mCamera->SetLocation(JVector3(0, 0, -10));
		TextureInfo textureInfo;

		textureInfo.Width = 1920;
		textureInfo.Height = 1080;
		textureInfo.ClearColor = Color::Green();
		textureInfo.Format = ETextureFormat::R8G8B8A8_Unorm;
		textureInfo.MipLevel = 1;
		textureInfo.Flags = ETextureFlags::Allow_RenderTarget;
		textureInfo.ArraySize = 1;
		mRenderTexture = ITexture::Create(TT("DevLayer_Texture"), textureInfo);
		mFrameBuffer->SubmitTexture(mRenderTexture);
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
	bool DevLayer::Resize(AppResizeEvent& e)
	{
		return true;
	}
}