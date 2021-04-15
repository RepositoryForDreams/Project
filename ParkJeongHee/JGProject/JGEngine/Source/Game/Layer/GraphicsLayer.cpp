#include "pch.h"
#include "GraphicsLayer.h"
#include "Application.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Class/Asset/AssetImporter.h"
namespace JG
{
	void GraphicsLayer::OnAttach()
	{

	}

	void GraphicsLayer::OnDetach()
	{

	}

	void GraphicsLayer::Begin()
	{
		mCamera = Camera::Create(JVector2(800, 600), Math::ConvertToRadians(90), 0.1f, 1000.0f, false);
		mCamera->SetLocation(JVector3(0, 0, -10));
		TextureInfo textureInfo;

		textureInfo.Width = 800;
		textureInfo.Height = 600;
		textureInfo.ClearColor = Color();
		textureInfo.Format = ETextureFormat::R8G8B8A8_Unorm;
		textureInfo.MipLevel = 1;
		textureInfo.Flags = ETextureFlags::Allow_RenderTarget;
		textureInfo.ArraySize = 1;
		auto texture = ITexture::Create(TT("Test_Texture"), textureInfo);
		mCamera->SetTargetTexture(texture);


		


		auto meshPath = CombinePath(Application::GetAssetPath(), TT("Samples/Model/Sphere.mesh"));
		mMesh = IMesh::CreateFromFile(meshPath);


		Scheduler::GetInstance().Schedule(0, 0.16f, -1, SchedulePriority::Default, SCHEDULE_BIND_FN(&GraphicsLayer::Update));
		

		mShader = IShader::Create(TT("Test_3DRender"),
			TT(R"(
SamplerState gPointSampler
{
	Template = Point_Wrap
};
cbuffer Object
{
	float4x4 gWorld;
};

cbuffer Camera
{
	float4x4 gViewProj;
};

struct VS_IN
{
	float3 posL  : POSITION;
	float3 normal    : NORMAL;
	float3 tangent   : TANGENT;
	float3 bitangent : BITANGENT;
	float2 tex  : TEXCOORD;
};
struct VS_OUT
{
	float4 posH       : SV_POSITION;
	float3 posW       : POSITION;
	float3 normalW    : NORMAL;
	float3 tangentW   : TANGENT;
	float3 bitangentW : BITANGENT;
	float2 tex : TEXCOORD;
};

VS_OUT vs_main(VS_IN vin)
{
	VS_OUT vout;
    float4 posW = mul(float4(vin.posL, 1.0f), gWorld);
	vout.posH = mul(posW, gViewProj);
	vout.posW = posW.xyz;
	vout.normalW    = mul(float4(vin.normal, 1.0f), gWorld).xyz;
	vout.tangentW   = mul(float4(vin.tangent, 1.0f), gWorld).xyz;
	vout.bitangentW = mul(float4(vin.bitangent, 1.0f), gWorld).xyz;
	vout.tex = vin.tex;



	vout.normalW = normalize(vout.normalW);
	vout.tangentW = normalize(vout.tangentW);
	vout.bitangentW = normalize(vout.bitangentW);
	return vout;
}
float4 ps_main(VS_OUT pin) : SV_TARGET
{
	return float4(1.0f,1.0f,1.0f,1.0f);
}
)"), EShaderFlags::Allow_PixelShader | EShaderFlags::Allow_VertexShader);

		mMaterial = IMaterial::Create(TT("Material"), mShader);
		if (mMaterial->SetFloat4x4(TT("gWorld"), JMatrix::Identity()) == false)
		{
			JG_WARN("Failed SetFloat4x4 gWorld");
		}
		if (mMaterial->SetFloat4x4(TT("gViewProj"), JMatrix::Transpose(mCamera->GetViewProjMatrix())) == false)
		{
			JG_WARN("Failed SetFloat4x4 gViewProj");
		}

		mTestTexture = ITexture::CreateFromFile(CombinePath(Application::GetAssetPath(), TT("Samples/Texture/T_Wood_Pine_D.TGA")));
	}

	void GraphicsLayer::Destroy()
	{

	}

	void GraphicsLayer::OnEvent(IEvent& e)
	{
	}

	String GraphicsLayer::GetLayerName()
	{
		return TT("GraphicsLayer");
	}

	EScheduleResult GraphicsLayer::Update()
	{
		if (Renderer3D::Begin(mCamera))
		{
			Renderer3D::DrawCall(mMesh, mMaterial);

			Renderer3D::End();
		}

		//mCamera->SetOrthographic(true);
		//if (Renderer2D::Begin(mCamera))
		//{
		//	Renderer2D::DrawCall(JVector2(0, 0), JVector2(200, 200), mTestTexture);

		//	Renderer2D::End();
		//}

		return EScheduleResult::Continue;
	}
}

