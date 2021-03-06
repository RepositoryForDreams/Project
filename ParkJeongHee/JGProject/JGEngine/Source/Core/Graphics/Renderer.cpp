#include "pch.h"
#include "Renderer.h"
#include "Application.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Material.h"
#include "Graphics/Camera.h"
#include "GraphicsAPI.h"

namespace JG
{
	struct QuadVertex
	{
		JVector3 Pos;
		JVector2 Tex;
		QuadVertex() = default;
		QuadVertex(const JVector3& pos, const JVector2& tex) : Pos(pos), Tex(tex) {}
	};
	struct Renderer2DItem
	{
		static const u32 MaxQuadCount    = 1200;
		static const u32 MaxTextureCount = MaxQuadCount;
		static const u32 MaxVertexCount  = MaxQuadCount * 4;
		static const u32 MaxIndexCount   = MaxQuadCount * 6;


		static const u32 QuadVertexCount = 4;
		static const u32 QuadIndexCount = 6;

		JVector3 StandardQuadPosition[4];
		JVector2 StandardQuadTexcoord[4];


		SharedPtr<IMesh> QuadMesh; // Bind
		SharedPtr<IVertexBuffer> QuadVBuffer;
		SharedPtr<IIndexBuffer>  QuadIBuffer;

		SharedPtr<IShader>   Standard2DShader;
		SharedPtr<IMaterial> Standard2DMaterial; // Bind
		List<QuadVertex> Vertices;
		List<u32>        Indices;


		SharedPtr<ITexture> DepthTexture;


		u32 QuadCount = 0;
	};
	UniquePtr<Renderer2DItem> gRenderer2DItem;

	bool Renderer2D::Create()
	{
		gRenderer2DItem = CreateUniquePtr<Renderer2DItem>();
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
StructuredBuffer<uint> gTextureIndexArray;

cbuffer Camera
{
	float4x4 gViewProj;
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
    
	vout.posH = mul(float4(vin.posL, 1.0f), gViewProj);
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


		gRenderer2DItem->StandardQuadPosition[0] = JVector3(-0.5f, -0.5f, 0.0f);
		gRenderer2DItem->StandardQuadPosition[1] = JVector3(-0.5f, +0.5f, 0.0f);
		gRenderer2DItem->StandardQuadPosition[2] = JVector3(+0.5f, +0.5f, 0.0f);
		gRenderer2DItem->StandardQuadPosition[3] = JVector3(+0.5f, -0.5f, 0.0f);

		gRenderer2DItem->StandardQuadTexcoord[0] = JVector2(0.0f, 1.0f);
		gRenderer2DItem->StandardQuadTexcoord[1] = JVector2(0.0f, 0.0f);
		gRenderer2DItem->StandardQuadTexcoord[2] = JVector2(1.0f, 0.0f);
		gRenderer2DItem->StandardQuadTexcoord[3] = JVector2(1.0f, 1.0f);



		gRenderer2DItem->Vertices.resize(Renderer2DItem::MaxVertexCount, QuadVertex());
		gRenderer2DItem->Indices.resize(Renderer2DItem::MaxIndexCount, 0);

		u32 offset = 0;
		for (u32 i = 0; i < Renderer2DItem::MaxIndexCount;)
		{
	
			gRenderer2DItem->Indices[i++] = offset + 0;
			gRenderer2DItem->Indices[i++] = offset + 1;
			gRenderer2DItem->Indices[i++] = offset + 2;
			gRenderer2DItem->Indices[i++] = offset + 0;
			gRenderer2DItem->Indices[i++] = offset + 2;
			gRenderer2DItem->Indices[i++] = offset + 3;
			offset += Renderer2DItem::QuadVertexCount;
		}



		TextureInfo textureInfo;
		textureInfo.Width = 1920;
		textureInfo.Height = 1080;
		textureInfo.Format = ETextureFormat::D24_Unorm_S8_Uint;
		textureInfo.MipLevel = 1;
		textureInfo.Flags = ETextureFlags::Allow_DepthStencil;
		textureInfo.ArraySize = 1;

		gRenderer2DItem->DepthTexture = ITexture::Create(TT("DepthTexture"), textureInfo);
		return true;
	}
	void Renderer2D::Destroy()
	{
		gRenderer2DItem.reset();
		gRenderer2DItem = nullptr;
	}
	bool Renderer2D::Begin(SharedPtr<Camera> camera, SharedPtr<ITexture> renderTexture)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		if (renderTexture == nullptr || camera == nullptr)
		{
			return false;
		}
		// TODO Camera 

		auto resolution = camera->GetResolution();
		api->SetViewports({ Viewport (resolution.x, resolution.y)});
		api->SetScissorRects({ ScissorRect(0,0, resolution.x,resolution.y) });
		api->ClearRenderTarget({ renderTexture }, gRenderer2DItem->DepthTexture);
		api->SetRenderTarget({ renderTexture }, gRenderer2DItem->DepthTexture);



		auto viewProj = JMatrix::Transpose(camera->GetViewProjMatrix());
		if (gRenderer2DItem->Standard2DMaterial->SetFloat4x4(TT("gViewProj"), viewProj) == false)
		{
			JG_CORE_ERROR("Failed Set ViewProjMatrix in Renderer2D");
			return false;
		}
		return true;
	}

	void Renderer2D::DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<ITexture> texture, const Color& color)
	{
		if (gRenderer2DItem->QuadCount >= Renderer2DItem::MaxQuadCount)
		{
			NextBatch();
		}


		JMatrix worldMatrix =
			JMatrix::Scaling(JVector3(Size, 1.0f)) * JMatrix::Rotation(JVector3(0.0f, 0.0f, Math::ConvertToRadians(rotation))) * JMatrix::Translation(JVector3(Pos, 0.0f));

		 
		u32 offset = gRenderer2DItem->QuadCount * Renderer2DItem::QuadVertexCount;
		for (u32 i = 0; i < Renderer2DItem::QuadVertexCount; ++i)
		{
			u32 index = offset + i;
			gRenderer2DItem->Vertices[index].Pos = worldMatrix.TransformPoint(gRenderer2DItem->StandardQuadPosition[i]);
			gRenderer2DItem->Vertices[index].Tex = gRenderer2DItem->StandardQuadTexcoord[i];
		}
		gRenderer2DItem->QuadCount++;
	}

	void Renderer2D::DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<IMaterial> material)
	{
		//

	}

	void Renderer2D::End()
	{
		NextBatch();
	}

	void Renderer2D::StartBatch()
	{
		gRenderer2DItem->QuadCount = 0;
	}

	void Renderer2D::NextBatch()
	{
		if (gRenderer2DItem->QuadCount == 0) return;

		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");



		if (gRenderer2DItem->Standard2DMaterial->Bind() == false)
		{
			JG_CORE_ERROR("Failed Bind StandardMaterial");
			return;
		}

		u32 quadVertexCount = gRenderer2DItem->QuadCount * Renderer2DItem::QuadVertexCount;
		u32 quadIndexCount = gRenderer2DItem->QuadCount * Renderer2DItem::QuadIndexCount;


		gRenderer2DItem->QuadVBuffer->SetData(gRenderer2DItem->Vertices.data(), sizeof(QuadVertex), quadVertexCount);
		gRenderer2DItem->QuadIBuffer->SetData(gRenderer2DItem->Indices.data(), quadIndexCount);
		if (gRenderer2DItem->QuadMesh->Bind() == false)
		{
			JG_CORE_ERROR("Failed Bind QuadMesh");
			return;
		}


		api->DrawIndexed(quadIndexCount);

		StartBatch();
	}


}