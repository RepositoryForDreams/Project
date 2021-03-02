#include "DevLayer.h"
#include "Application.h"
#include "Platform/Window/Window.h"
#include "Graphics/GraphicsAPI.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
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
		if (Renderer2D::Begin())
		{





			auto resultTexture = Renderer2D::End();

			mFrameBuffer->SubmitTexture(resultTexture);
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