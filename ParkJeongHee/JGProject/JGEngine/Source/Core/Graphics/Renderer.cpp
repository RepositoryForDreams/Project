#include "pch.h"
#include "Renderer.h"
#include "Application.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Material.h"
#include "GraphicsAPI.h"

namespace JG
{
	struct QuadVertex
	{
		JVector3 Pos;
		JVector2 Tex;

		QuadVertex(const JVector3& pos, const JVector2& tex) : Pos(pos), Tex(tex) {}
	};
	struct Renderer2DItem
	{
		SharedPtr<ITexture> RenderTexture;
		SharedPtr<ITexture> DepthTexture;


		SharedPtr<IMesh> QuadMesh; // Bind
		SharedPtr<IVertexBuffer> QuadVBuffer;
		SharedPtr<IIndexBuffer>  QuadIBuffer;

		SharedPtr<IShader>   Standard2DShader;
		SharedPtr<IMaterial> Standard2DMaterial; // Bind
		List<QuadVertex> Vertices;
		List<u32>        Indices;
	};
	UniquePtr<Renderer2DItem> gRenderer2DItem;

	bool Renderer2D::Create()
	{
		gRenderer2DItem = CreateUniquePtr<Renderer2DItem>();

		// RenderTexture
		{
			TextureInfo info;
			info.Width  = 1920; info.Height = 1080; info.MipLevel = 1; info.ArraySize = 1;
			info.Format = ETextureFormat::R8G8B8A8_Unorm;
			info.Flags  = ETextureFlags::Allow_RenderTarget;
			gRenderer2DItem->RenderTexture = ITexture::Create(TT("Renderer2D_RenderTexture"), info);
		}
		// DepthTexture
		{
			TextureInfo info;
			info.Width = 1920; info.Height = 1080; info.MipLevel = 1; info.ArraySize = 1;
			info.Format = ETextureFormat::D24_Unorm_S8_Uint;
			info.Flags = ETextureFlags::Allow_DepthStencil;

			gRenderer2DItem->DepthTexture = ITexture::Create(TT("Renderer2D_DepthTexture"), info);
		}
		gRenderer2DItem->QuadMesh    = IMesh::Create(TT("Renderer2D_QuadMesh"));

		auto inputLayout = InputLayout::Create();
		inputLayout->Add(EShaderDataType::_float3, "POSITION", 0);
		inputLayout->Add(EShaderDataType::_float2, "TEXCOORD", 0);

		gRenderer2DItem->QuadMesh->SetInputLayout(inputLayout);




		gRenderer2DItem->QuadVBuffer = IVertexBuffer::Create(TT("Renderer2D_VBuffer"));
		gRenderer2DItem->QuadIBuffer = IIndexBuffer::Create(TT("Renderer2D_IBuffer"));
		gRenderer2DItem->QuadMesh->AddVertexBuffer(gRenderer2DItem->QuadVBuffer);
		gRenderer2DItem->QuadMesh->SetIndexBuffer(gRenderer2DItem->QuadIBuffer);

		gRenderer2DItem->Standard2DShader = IShader::Create(TT("Standard2DShader"),
			TT(R"(
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
)"), EShaderFlags::Allow_VertexShader | EShaderFlags::Allow_PixelShader);
		ShaderLibrary::RegisterShader(gRenderer2DItem->Standard2DShader);

		gRenderer2DItem->Standard2DMaterial = IMaterial::Create(TT("Standard2DMaterial"),gRenderer2DItem->Standard2DShader);

		return true;
	}
	void Renderer2D::Destroy()
	{
		gRenderer2DItem.reset();
		gRenderer2DItem = nullptr;
	}
	bool Renderer2D::Begin()
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");


		gRenderer2DItem->Vertices.clear();
		gRenderer2DItem->Indices.clear();
		api->ClearRenderTarget({ gRenderer2DItem->RenderTexture }, gRenderer2DItem->DepthTexture);
		api->SetRenderTarget({ gRenderer2DItem->RenderTexture }, gRenderer2DItem->DepthTexture);


		if (gRenderer2DItem->Standard2DMaterial->SetFloat4x4(TT("gWorld"), JMatrix::Identity()) == false)
		{
			JG_CORE_ERROR("Faeild Set WorldMatrix in Renderer2D");
			return false;
		}
		auto view = JMatrix::LookAtLH(JVector3(0, 0, -10), JVector3(0, 0, 1), JVector3(0, 1, 0));
		auto proj = JMatrix::OrthographicLH(1920, 1080, 0.1f, 1000.0f);
		auto viewProj = JMatrix::Transpose(view * proj);
		if (gRenderer2DItem->Standard2DMaterial->SetFloat4x4(TT("gViewProj"), viewProj) == false)
		{
			JG_CORE_ERROR("Faeild Set ViewProjMatrix in Renderer2D");
			return false;
		}
		if (gRenderer2DItem->Standard2DMaterial->Bind() == false)
		{
			JG_CORE_ERROR("Faeild Bind StandardMaterial");
			return false;
		}
		
		return true;
	}

	void Renderer2D::DrawQuad(const JVector2& Pos, const JVector2& Size, const Color& color)
	{
		float hw  = Size.x * 0.5f;
		float hh  = Size.y * 0.5f;

		gRenderer2DItem->Vertices.push_back(QuadVertex(JVector3(-hw, -hh, 0.0f), JVector2(0.0f, 1.0f)));
		gRenderer2DItem->Vertices.push_back(QuadVertex(JVector3(-hw, +hh, 0.0f), JVector2(0.0f, 0.0f)));
		gRenderer2DItem->Vertices.push_back(QuadVertex(JVector3(+hw, +hh, 0.0f), JVector2(1.0f, 0.0f)));
		gRenderer2DItem->Vertices.push_back(QuadVertex(JVector3(+hw, -hh, 0.0f), JVector2(1.0f, 1.0f)));

		gRenderer2DItem->Indices.push_back(0); gRenderer2DItem->Indices.push_back(1); gRenderer2DItem->Indices.push_back(2);
		gRenderer2DItem->Indices.push_back(0); gRenderer2DItem->Indices.push_back(2); gRenderer2DItem->Indices.push_back(3);
	}

	SharedPtr<ITexture> Renderer2D::End()
	{
		DrawQuad(JVector2(0, 0), JVector2(100, 100));
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");


		gRenderer2DItem->QuadVBuffer->Create(gRenderer2DItem->Vertices.data(), sizeof(QuadVertex), gRenderer2DItem->Vertices.size());
		gRenderer2DItem->QuadIBuffer->Create(gRenderer2DItem->Indices.data(), gRenderer2DItem->Indices.size());


		
		gRenderer2DItem->QuadMesh->Bind();


		api->DrawIndexed(gRenderer2DItem->Indices.size());




		return gRenderer2DItem->RenderTexture;
	}


}