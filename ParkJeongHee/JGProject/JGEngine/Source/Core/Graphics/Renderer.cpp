#include "pch.h"
#include "Renderer.h"
#include "Application.h"

#include "Class/Game/Components/Camera.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Material.h"
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

	std::atomic<ERendererState> RendererState::mState = ERendererState::Wait;

	bool Renderer::Create()
	{
		return true;
	}
	void Renderer::Destroy()
	{

	}
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
		RendererState::SetState(ERendererState::Wait);
	}




	Renderer2D::Renderer2D()
	{
		mQuadMesh = IMesh::Create(TT("Renderer2D_QuadMesh"));
		
		auto inputLayout = InputLayout::Create();
		inputLayout->Add(EShaderDataType::_float3, "POSITION", 0);
		inputLayout->Add(EShaderDataType::_float2, "TEXCOORD", 0);
		inputLayout->Add(EShaderDataType::_float4, "COLOR", 0);
		inputLayout->Add(EShaderDataType::_int, "TEXTUREINDEX", 0);
		mQuadMesh->SetInputLayout(inputLayout);
		
		mQuadVBuffer = IVertexBuffer::Create(TT("Renderer2D_VBuffer"), EBufferLoadMethod::CPULoad);
		mQuadIBuffer = IIndexBuffer::Create(TT("Renderer2D_IBuffer"), EBufferLoadMethod::CPULoad);
		mQuadMesh->AddVertexBuffer(mQuadVBuffer);
		mQuadMesh->SetIndexBuffer(mQuadIBuffer);
		
		mStandard2DShader = ShaderLibrary::GetInstance().Get(ShaderScript::Standard2DShader);
		mStandard2DMaterial = IMaterial::Create(TT("Standard2DMaterial"), mStandard2DShader);
		mStandardQuadPosition[0] = JVector3(-0.5f, -0.5f, 0.0f);
		mStandardQuadPosition[1] = JVector3(-0.5f, +0.5f, 0.0f);
		mStandardQuadPosition[2] = JVector3(+0.5f, +0.5f, 0.0f);
		mStandardQuadPosition[3] = JVector3(+0.5f, -0.5f, 0.0f);
		mStandardQuadTexcoord[0] = JVector2(0.0f, 1.0f);
		mStandardQuadTexcoord[1] = JVector2(0.0f, 0.0f);
		mStandardQuadTexcoord[2] = JVector2(1.0f, 0.0f);
		mStandardQuadTexcoord[3] = JVector2(1.0f, 1.0f);

		mVertices.resize(MaxVertexCount, QuadVertex());
		mIndices.resize(MaxIndexCount, 0);

		u32 offset = 0;
		for (u32 i = 0; i < MaxIndexCount;)
		{
			mIndices[i++] = offset + 0;
			mIndices[i++] = offset + 1;
			mIndices[i++] = offset + 2;
			mIndices[i++] = offset + 0;
			mIndices[i++] = offset + 2;
			mIndices[i++] = offset + 3;
			offset += QuadVertexCount;
		}
		mTextureArray.resize(MaxTextureCount, nullptr);


		TextureInfo textureInfo;
		textureInfo.Width = 1; textureInfo.Height = 1; 	textureInfo.MipLevel = 1; 	textureInfo.ArraySize = 1;
		textureInfo.ClearColor = Color::White();
		textureInfo.Format = ETextureFormat::R8G8B8A8_Unorm; textureInfo.Flags = ETextureFlags::Allow_RenderTarget;
		mWhiteTexture = ITexture::Create(TT("WhiteTexture"), textureInfo);
		mTextureArray[0] = mWhiteTexture;

		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		api->ClearRenderTarget({ mWhiteTexture }, nullptr);


		if (mStandard2DMaterial->SetTexture(TT("gTexture"), 0, mWhiteTexture) == false)
		{
			JG_CORE_ERROR("Failed Set Texture in WhiteTexture");
			return;
		}
		mStandard2DMaterial->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
		mStandard2DMaterial->SetBlendState(0, EBlendStateTemplate::Transparent_Default);

		StartBatch();
	}

	Renderer2D::~Renderer2D()
	{

	}

	bool Renderer2D::Begin(SharedPtr<Camera> camera)
	{
		if (camera == nullptr)
		{
			return false;
		}
		return Begin(camera->GetResolution(), camera->GetViewProjMatrix(), camera->GetTargetTexture());
	}
	bool Renderer2D::Begin(const JVector2& resolution, const JMatrix& viewProj, SharedPtr<ITexture> targetTexture)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		api->SetViewports({ Viewport(resolution.x, resolution.y) });
		api->SetScissorRects({ ScissorRect(0,0, resolution.x,resolution.y) });
		api->ClearRenderTarget({ targetTexture }, nullptr);
		api->SetRenderTarget({ targetTexture }, nullptr);

		auto transposedViewProj = JMatrix::Transpose(viewProj);
		if (mStandard2DMaterial->SetFloat4x4(ShaderScript::Standard2D::ViewProj, transposedViewProj) == false)
		{
			JG_CORE_ERROR("Failed Set ViewProjMatrix in Renderer2D");
			return false;
		}
		return true;
	}
	void Renderer2D::End()
	{
		NextBatch();
	}


	void Renderer2D::DrawCall(const JMatrix& transform, SharedPtr<ITexture> texture, const Color& color)
	{
		if (mQuadCount >= MaxQuadCount || mTextureCount >= MaxTextureCount)
		{
			NextBatch();
		}
		i32 textureIndex = 0;
		if (texture != nullptr && texture->IsValid() == true)
		{
			for (auto& tex : mTextureArray)
			{
				if (tex == nullptr || tex->GetTextureID() == texture->GetTextureID())
				{
					mTextureCount++;
					mTextureArray[textureIndex] = texture;
					if (mStandard2DMaterial->SetTexture(ShaderScript::Standard2D::Texture, textureIndex, mTextureArray[textureIndex]) == false)
					{
						JG_CORE_ERROR("Failed Set Texture Slot : {0}", textureIndex);
					}
					break;
				}
				++textureIndex;
			}
		}

		u32 offset = mQuadCount * QuadVertexCount;
		for (u32 i = 0; i < QuadVertexCount; ++i)
		{
			u32 index = offset + i;
			mVertices[index].Pos = transform.TransformPoint(mStandardQuadPosition[i]);
			mVertices[index].Tex = mStandardQuadTexcoord[i];
			mVertices[index]._Color = color;
			mVertices[index].TextureIndex = textureIndex;
		}
		mQuadCount++;
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

	void Renderer2D::StartBatch()
	{
		mQuadCount = 0;
		mTextureCount = 1;
	}

	void Renderer2D::NextBatch()
	{
		if (mQuadCount == 0) return;
		
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		if (mStandard2DMaterial->Bind() == false)
		{
			JG_CORE_ERROR("Failed Bind StandardMaterial");
			StartBatch();
			return;
		}
		
		u32 quadVertexCount = mQuadCount * QuadVertexCount;
		u32 quadIndexCount = mQuadCount * QuadIndexCount;
		
		
		mQuadVBuffer->SetData(mVertices.data(), sizeof(QuadVertex), quadVertexCount);
		mQuadIBuffer->SetData(mIndices.data(), quadIndexCount);
		if (mQuadMesh->Bind() == false)
		{
			JG_CORE_ERROR("Failed Bind QuadMesh");
			StartBatch();
			return;
		}
		
		
		api->DrawIndexed(quadIndexCount);
		StartBatch();
	}
}