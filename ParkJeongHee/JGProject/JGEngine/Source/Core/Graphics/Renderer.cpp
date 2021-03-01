#include "pch.h"
#include "Renderer.h"
#include "Application.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
#include "GraphicsAPI.h"

namespace JG
{
	struct Vertex
	{
		JVector3 Pos;
		JVector2 Tex;
	};
	struct Renderer2DItem
	{
		SharedPtr<ITexture> RenderTexture;
		SharedPtr<ITexture> DepthTexture;


		SharedPtr<IMesh> QuadMesh;
		SharedPtr<IVertexBuffer> QuadVBuffer;
		SharedPtr<IIndexBuffer>  QuadIBuffer;

		SharedPtr<IShader>   Standard2DShader;
		SharedPtr<IMaterial> Standard2DMaterial;
		List<Vertex> Vertices;
		List<u32>    Indices;
	};
	UniquePtr<Renderer2DItem> gRenderer2DItem;

	bool Renderer2D::Create()
	{
		gRenderer2DItem = CreateUniquePtr<Renderer2DItem>();

		// RenderTexture
		{
			TextureInfo info;
			info.Width  = 1; info.Height = 1; info.MipLevel = 1; info.ArraySize = 1;
			info.Format = ETextureFormat::R8G8B8A8_Unorm;
			info.Flags  = ETextureFlags::Allow_RenderTarget;
			gRenderer2DItem->RenderTexture = ITexture::Create(TT("Renderer2D_RenderTexture"), info);
		}
		// DepthTexture
		{
			TextureInfo info;
			info.Width = 1; info.Height = 1; info.MipLevel = 1; info.ArraySize = 1;
			info.Format = ETextureFormat::D24_Unorm_S8_Uint;
			info.Flags = ETextureFlags::Allow_DepthStencil;

			gRenderer2DItem->DepthTexture = ITexture::Create(TT("Renderer2D_DepthTexture"), info);
		}
		gRenderer2DItem->QuadMesh = IMesh::Create(TT("Renderer2D_QuadMesh"));
		gRenderer2DItem->QuadVBuffer = IVertexBuffer::Create(TT("Renderer2D_VBuffer"));
		gRenderer2DItem->QuadIBuffer = IIndexBuffer::Create(TT("Renderer2D_IBuffer"));



		return true;
	}
	void Renderer2D::Destroy()
	{
		gRenderer2DItem.reset();
		gRenderer2DItem = nullptr;
	}
	bool Renderer2D::Begin()
	{
		


		return true;
	}

	SharedPtr<ITexture> Renderer2D::End()
	{


		return nullptr;
	}


}