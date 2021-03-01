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
		FrameBufferInfo info = FrameBufferInfo(
			Application::GetInstance().GetWindow()->GetHandle(),
			Color::Red(),
			ETextureFormat::R8G8B8A8_Unorm,
			1920, 1080);

		mFrameBuffer = IFrameBuffer::Create(info);

		String error;
		IShader::Create(TT(R"(
cbuffer Camera
{
	float4x4 gViewProj;
};


cbuffer Instance
{
	float4x4 gWorld;
};


struct VS_IN
{
	float3 posL : POSITION;
	float2 tex  : TEXCOORD;
};

struct VS_OUT
{
	float4 posH : SV_POSITION;
	float2 tex   : TEXCOORD;
};

VS_OUT vs_main(VS_IN vin)
{
	VS_OUT vout;
	float4 posW = mul(float4(vin.posL, 1.0f), gWorld);


	vout.posH = mul(posW, gViewProj);
	vout.tex = vin.tex;
	return vout;
}

float4 ps_main(VS_OUT pin) : SV_TARGET
{

	return float4(1.0f,1.0f,1.0f,1.0f);
}
)"), EShaderFlags::Allow_PixelShader | EShaderFlags::Allow_VertexShader);
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