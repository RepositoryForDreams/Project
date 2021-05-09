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
	/* Renderer State
	* 
	
	*/
	enum class ERendererState
	{
		Wait,
		Run
	};
	struct RendererState
	{
		static std::atomic<ERendererState> mState;
		static std::mutex mMutex;

	public:
		static void SetState(ERendererState state)
		{
			mState = state;
		}
		static ERendererState GetState() {
			return mState;
		}

		static bool IsRunable() {
			return mState == ERendererState::Wait;
		}

	};

	/* Renderer 2D Pipeline */
	struct QuadVertex
	{
		JVector3 Pos;
		JVector2 Tex;
		Color _Color = Color::White();
		i32 TextureIndex = 0;
		QuadVertex() = default;
		QuadVertex(const JVector3& pos, const JVector2& tex) : Pos(pos), Tex(tex) {}
		QuadVertex(const JVector3& pos, const JVector2& tex, const Color& color, i32 textureIndex)
			: Pos(pos), Tex(tex), _Color(color), TextureIndex(textureIndex) {}
	};
	struct Renderer2DItem
	{
		static const u32 MaxQuadCount = 1200;
		static const u32 MaxVertexCount = MaxQuadCount * 4;
		static const u32 MaxIndexCount = MaxQuadCount * 6;
		static const u32 MaxTextureCount = 64;

		static const u32 QuadVertexCount = 4;
		static const u32 QuadIndexCount = 6;

		JVector3 StandardQuadPosition[4];
		JVector2 StandardQuadTexcoord[4];

		Renderer2D::Statistics Statistics;
		SharedPtr<IMesh> QuadMesh; // Bind
		SharedPtr<IVertexBuffer> QuadVBuffer;
		SharedPtr<IIndexBuffer>  QuadIBuffer;

		SharedPtr<IShader>   Standard2DShader;
		SharedPtr<IMaterial> Standard2DMaterial; // Bind
		List<QuadVertex> Vertices;
		List<u32>        Indices;
		List<SharedPtr<ITexture>> TextureArray;
		SharedPtr<ITexture> WhiteTexture;



		u32 TextureCount = 1;
		u32 QuadCount = 0;

		u32 TotalQuadCount = 0;
		u32 DrawCall = 0;
	};
	UniquePtr<Renderer2DItem> gRenderer2DItem;
	std::atomic<ERendererState> RendererState::mState = ERendererState::Wait;



	bool Renderer::Create()
	{
		if (Renderer2D::Create() == false)
		{
			JG_CORE_CRITICAL("Failed Create Renderer2D");
			return false;
		}

		return true;
	}
	void Renderer::Destroy()
	{
		Renderer2D::Destroy();
	}
	/* Renderer 3D Pipeline

	*/
	bool Renderer::Begin(SharedPtr<Camera> camera)
	{
		if (RendererState::IsRunable() == false)
		{
			return false;
		}

		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		if (camera == nullptr)
		{
			return false;
		}
		RendererState::SetState(ERendererState::Run);
		// TODO Camera 
		auto targetTextures = camera->GetTargetTextures();
		auto depthTargetTexture = camera->GetTargetDepthTexture();


		auto resolution = camera->GetResolution();
		api->SetViewports({ Viewport(resolution.x, resolution.y) });
		api->SetScissorRects({ ScissorRect(0,0, resolution.x,resolution.y) });
		api->ClearRenderTarget(targetTextures, depthTargetTexture);
		api->SetRenderTarget(targetTextures, depthTargetTexture);

		auto viewProj = JMatrix::Transpose(camera->GetViewProjMatrix());
		if (gRenderer2DItem->Standard2DMaterial->SetFloat4x4(TT("gViewProj"), viewProj) == false)
		{
			JG_CORE_ERROR("Failed Set ViewProjMatrix in Renderer2D");
			return false;
		}
		return true;
	}

	void Renderer::DrawCall(SharedPtr<IMesh> mesh, SharedPtr<IMaterial> material)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");


		if (material->Bind() == false)
		{
			return;
		}
		if (mesh->Bind() == false)
		{
			return;
		}

		api->DrawIndexed(mesh->GetIndexCount());
	}

	void Renderer::End()
	{
		Renderer2D::NextBatch();
		RendererState::SetState(ERendererState::Wait);
	}




	

	bool Renderer2D::Create()
	{
		gRenderer2DItem = CreateUniquePtr<Renderer2DItem>();
		gRenderer2DItem->QuadMesh    = IMesh::Create(TT("Renderer2D_QuadMesh"));

		auto inputLayout = InputLayout::Create();
		inputLayout->Add(EShaderDataType::_float3, "POSITION", 0);
		inputLayout->Add(EShaderDataType::_float2, "TEXCOORD", 0);
		inputLayout->Add(EShaderDataType::_float4, "COLOR", 0);
		inputLayout->Add(EShaderDataType::_int, "TEXTUREINDEX", 0);
		gRenderer2DItem->QuadMesh->SetInputLayout(inputLayout);


		gRenderer2DItem->QuadVBuffer = IVertexBuffer::Create(TT("Renderer2D_VBuffer"), EBufferLoadMethod::CPULoad);
		gRenderer2DItem->QuadIBuffer = IIndexBuffer::Create(TT("Renderer2D_IBuffer"), EBufferLoadMethod::CPULoad);
		gRenderer2DItem->QuadMesh->AddVertexBuffer(gRenderer2DItem->QuadVBuffer);
		gRenderer2DItem->QuadMesh->SetIndexBuffer(gRenderer2DItem->QuadIBuffer);

		// 2DShader
		{
			gRenderer2DItem->Standard2DShader = IShader::Create(TT("Standard2DShader"),
				TT(R"(
SamplerState gPointSampler
{
	Template = Point_Wrap
};

Texture2D gTexture[64];

cbuffer Camera
{
	float4x4 gViewProj;
};

struct VS_IN
{
	float3 posL : POSITION;
	float2 tex  : TEXCOORD;
	float4 color : COLOR;
	int textureIndex : TEXTUREINDEX;
};
struct VS_OUT
{
	float4 posH : SV_POSITION;
	float2 tex   : TEXCOORD;
	float4 color : COLOR;
	int textureIndex : TEXTUREINDEX;
};

VS_OUT vs_main(VS_IN vin)
{
	VS_OUT vout;
    
	vout.posH = mul(float4(vin.posL, 1.0f), gViewProj);
	vout.tex = vin.tex;
	vout.color = vin.color;
	vout.textureIndex = vin.textureIndex;
	return vout;
}
float4 ps_main(VS_OUT pin) : SV_TARGET
{
	return gTexture[pin.textureIndex].Sample(gPointSampler, pin.tex) * pin.color;
}
)"), EShaderFlags::Allow_VertexShader | EShaderFlags::Allow_PixelShader);
		}

		//ShaderLibrary::RegisterShader(gRenderer2DItem->Standard2DShader);

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
		gRenderer2DItem->TextureArray.resize(Renderer2DItem::MaxTextureCount, nullptr);


		TextureInfo textureInfo;
		textureInfo.Width = 1; textureInfo.Height = 1; 	textureInfo.MipLevel = 1; 	textureInfo.ArraySize = 1;
		textureInfo.ClearColor = Color::White();
		textureInfo.Format = ETextureFormat::R8G8B8A8_Unorm; textureInfo.Flags = ETextureFlags::Allow_RenderTarget;
		gRenderer2DItem->WhiteTexture = ITexture::Create(TT("WhiteTexture"), textureInfo);
		gRenderer2DItem->TextureArray[0] = gRenderer2DItem->WhiteTexture;

		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		api->ClearRenderTarget({ gRenderer2DItem->WhiteTexture }, nullptr);


		if (gRenderer2DItem->Standard2DMaterial->SetTexture(TT("gTexture"), 0, gRenderer2DItem->WhiteTexture) == false)
		{
			JG_CORE_ERROR("Failed Set Texture in WhiteTexture");
			return false;
		}
		gRenderer2DItem->Standard2DMaterial->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
		gRenderer2DItem->Standard2DMaterial->SetBlendState(0, EBlendStateTemplate::Transparent_Default);
		
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, 0,
			[&]() -> EScheduleResult
		{
			gRenderer2DItem->Statistics.DrawCalls = gRenderer2DItem->DrawCall;
			gRenderer2DItem->Statistics.QuadCount = gRenderer2DItem->TotalQuadCount;

			gRenderer2DItem->DrawCall = 0;
			gRenderer2DItem->TotalQuadCount = 0;
			return EScheduleResult::Continue;
		});
		return true;
	}
	void Renderer2D::Destroy()
	{
		gRenderer2DItem.reset();
		gRenderer2DItem = nullptr;
	}

	void Renderer2D::DrawCall(const JMatrix& transform, SharedPtr<ITexture> texture, const Color& color)
	{
		if (gRenderer2DItem->QuadCount >= Renderer2DItem::MaxQuadCount || gRenderer2DItem->TextureCount >= Renderer2DItem::MaxTextureCount)
		{
			NextBatch();
		}
		i32 textureIndex = 0;
		if (texture != nullptr && texture->IsValid() == true)
		{
			for (auto& tex : gRenderer2DItem->TextureArray)
			{
				if (tex == nullptr || tex->GetTextureID() == texture->GetTextureID())
				{
					gRenderer2DItem->TextureCount++;
					gRenderer2DItem->TextureArray[textureIndex] = texture;
					if (gRenderer2DItem->Standard2DMaterial->SetTexture(TT("gTexture"), textureIndex, gRenderer2DItem->TextureArray[textureIndex]) == false)
					{
						JG_CORE_ERROR("Failed Set Texture Slot : {0}", textureIndex);
					}
					break;
				}
				++textureIndex;

			}
		}
	
		u32 offset = gRenderer2DItem->QuadCount * Renderer2DItem::QuadVertexCount;
		for (u32 i = 0; i < Renderer2DItem::QuadVertexCount; ++i)
		{
			u32 index = offset + i;
			gRenderer2DItem->Vertices[index].Pos = transform.TransformPoint(gRenderer2DItem->StandardQuadPosition[i]);
			gRenderer2DItem->Vertices[index].Tex = gRenderer2DItem->StandardQuadTexcoord[i];
			gRenderer2DItem->Vertices[index]._Color = color;
			gRenderer2DItem->Vertices[index].TextureIndex = textureIndex;
		}
		gRenderer2DItem->QuadCount++;
	}

	void Renderer2D::DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<ITexture> texture, const Color& color)
	{
		JMatrix transform =
			JMatrix::Scaling(JVector3(Size, 1.0f)) * JMatrix::Rotation(JVector3(0.0f, 0.0f, Math::ConvertToRadians(rotation))) * JMatrix::Translation(JVector3(Pos, 0.0f));

		DrawCall(transform, texture, color);

	}

	void Renderer2D::DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, const Color& color)
	{
		DrawCall(Pos, Size, rotation, nullptr, color);
	}

	void Renderer2D::DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<ITexture> texture)
	{
		DrawCall(Pos, Size, rotation, texture, Color::White());
	}

	void Renderer2D::DrawCall(const JVector2& Pos, const JVector2& Size, const Color& color)
	{
		DrawCall(Pos, Size, 0, color);
	}

	void Renderer2D::DrawCall(const JVector2& Pos, const JVector2& Size, SharedPtr<ITexture> texture)
	{
		DrawCall(Pos, Size, 0, texture);
	}

	//void Renderer2D::DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<IMaterial> material)
	//{
	//	//

	//}

	const Renderer2D::Statistics& Renderer2D::GetStats()
	{
		return gRenderer2DItem->Statistics;
	}

	void Renderer2D::StartBatch()
	{
		gRenderer2DItem->QuadCount = 0;
		gRenderer2DItem->TextureCount = 1;
	}

	void Renderer2D::NextBatch()
	{
		if (gRenderer2DItem->QuadCount == 0) return;

		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");



		if (gRenderer2DItem->Standard2DMaterial->Bind() == false)
		{
			JG_CORE_ERROR("Failed Bind StandardMaterial");
			StartBatch();
			return;
		}

		u32 quadVertexCount = gRenderer2DItem->QuadCount * Renderer2DItem::QuadVertexCount;
		u32 quadIndexCount = gRenderer2DItem->QuadCount * Renderer2DItem::QuadIndexCount;


		gRenderer2DItem->QuadVBuffer->SetData(gRenderer2DItem->Vertices.data(), sizeof(QuadVertex), quadVertexCount);
		gRenderer2DItem->QuadIBuffer->SetData(gRenderer2DItem->Indices.data(), quadIndexCount);
		if (gRenderer2DItem->QuadMesh->Bind() == false)
		{
			JG_CORE_ERROR("Failed Bind QuadMesh");
			StartBatch();
			return;
		}


		api->DrawIndexed(quadIndexCount);
		gRenderer2DItem->DrawCall++;
		gRenderer2DItem->TotalQuadCount += gRenderer2DItem->QuadCount;
		StartBatch();
	}




}