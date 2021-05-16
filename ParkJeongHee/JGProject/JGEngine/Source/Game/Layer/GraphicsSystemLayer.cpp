#include "pch.h"
#include "GraphicsSystemLayer.h"
#include "Application.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Resource.h"
#include "Graphics/GraphicsAPI.h"
#include "Class/Game/GameSettings.h"



namespace JG
{
	void GraphicsSystemLayer::OnAttach()
	{
		LoadShaderScript();
	}

	void GraphicsSystemLayer::OnDetach()
	{



	}

	void GraphicsSystemLayer::Begin()
	{
		Scheduler::GetInstance().Schedule(0, 0.02f, -1, SchedulePriority::EndSystem, SCHEDULE_BIND_FN(&GraphicsSystemLayer::Update));

		mMainCamera = Camera::Create(GameSettings::GetResolution(), Math::ConvertToRadians(90), 0.001f, 10000.0f, true);
		mMainCamera->SetLocation(JVector3(0, 0, -10));
		TextureInfo info = {};
		info.ArraySize = 1; info.Flags = ETextureFlags::Allow_RenderTarget;
		info.Format = ETextureFormat::R8G8B8A8_Unorm;
		info.Width  = mMainCamera->GetResolution().x;
		info.Height = mMainCamera->GetResolution().y;
		info.MipLevel = 1;
		auto mainTexture = ITexture::Create(TT("MainTexture"), info);
		mMainCamera->SetTargetTexture(mainTexture);
		mMainCamera->SetCullingLayerMask(JG_U64_MAX);

		mLayerMaterial = IMaterial::Create(TT("LayerMaterial"), ShaderLibrary::Get(ShaderScript::Standard2DShader));
		mLayerMaterial->SetFloat4x4(TT("gWorld"), JMatrix::Scaling(JVector3(info.Width, info.Height, 1.0f)));
		mLayerMaterial->SetFloat4(TT("gColor"), Color::White());
		mLayerMaterial->SetBlendState(0, EBlendStateTemplate::Transparent_Default);
		mLayerMaterial->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);




		TextureInfo textureInfo;
		textureInfo.Width = 1; textureInfo.Height = 1; 	textureInfo.MipLevel = 1; 	textureInfo.ArraySize = 1;
		textureInfo.ClearColor = Color::White();
		textureInfo.Format = ETextureFormat::R8G8B8A8_Unorm; textureInfo.Flags = ETextureFlags::Allow_RenderTarget;
		mNullTexture = ITexture::Create(TT("NullTexture"), textureInfo);

		//
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		api->ClearTexture(mNullTexture);
	}

	void GraphicsSystemLayer::Destroy()
	{
		mMainCamera = nullptr;
		mNullTexture = nullptr;
		mLayerMaterial = nullptr;
		mLayerCameras.clear();
		mPushedRenderItemList.clear();
		
	}

	void GraphicsSystemLayer::OnEvent(IEvent& e)
	{
		EventDispatcher eventDispatcher(e);
		eventDispatcher.Dispatch<RequestPushRenderItemEvent>(EVENT_BIND_FN(&GraphicsSystemLayer::ResponsePushRenderItem));
		eventDispatcher.Dispatch<RequestGetMainSceneTextureEvent>(EVENT_BIND_FN(&GraphicsSystemLayer::ResponseGetMainSceneTexture));
		eventDispatcher.Dispatch<RequestRegisterCameraEvent>(EVENT_BIND_FN(&GraphicsSystemLayer::ResponseRegisterCamera));
		eventDispatcher.Dispatch<RequestUnRegisterCameraEvent>(EVENT_BIND_FN(&GraphicsSystemLayer::ResponseUnRegisterCamera));

	}

	String GraphicsSystemLayer::GetLayerName()
	{
		return TT("GraphicsSystemLayer");
	}

	bool GraphicsSystemLayer::ResponsePushRenderItem(RequestPushRenderItemEvent& e)
	{
		if (e.RenderItem == nullptr)
		{
			return true;
		}

		mPushedRenderItemList.push_back(e.RenderItem);

		return true;
	}

	bool GraphicsSystemLayer::ResponseGetMainSceneTexture(RequestGetMainSceneTextureEvent& e)
	{
		e.SceneTexture = mMainCamera->GetTargetTexture();


		return true;
	}

	bool GraphicsSystemLayer::ResponseRegisterCamera(RequestRegisterCameraEvent& e)
	{
		if (mLayerCameras.find(e.SharedCamera.get()) != mLayerCameras.end())
		{
			return true;
		}
		mLayerCameras[e.SharedCamera.get()] = CreateLayerCamera(e.SharedCamera);

		return true;
	}

	bool GraphicsSystemLayer::ResponseUnRegisterCamera(RequestUnRegisterCameraEvent& e)
	{
		if (mLayerCameras.find(e.SharedCamera.get()) == mLayerCameras.end())
		{
			return true;
		}

		mLayerCameras.erase(e.SharedCamera.get());
		return true;
	}

	void GraphicsSystemLayer::Rendering(SharedPtr<Camera> camera, SharedPtr<IRenderItem> renderItem)
	{
		if (renderItem->Material == nullptr)
		{
			return;
		}
		renderItem->Material->SetFloat4x4(TT("gViewProj"), JMatrix::Transpose(camera->GetViewProjMatrix()));
		renderItem->Material->SetFloat4x4(TT("gWorld"), JMatrix::Transpose(renderItem->WorldMatrix));



		auto type = renderItem->GetType();
		

		if (type == JGTYPE(StandardSpriteRenderItem))
		{
			auto ri = static_cast<StandardSpriteRenderItem*>(renderItem.get());
			ri->Material->SetFloat4(TT("gColor"), ri->Color);
			if (ri->Texture == nullptr)
			{
				ri->Material->SetTexture(TT("gTexture"), 0, mNullTexture);
			}
			else
			{
				ri->Material->SetTexture(TT("gTexture"), 0, ri->Texture);
			}
		
			Renderer::DrawCall(ri->Mesh, ri->Material);
		}
		else
		{

		}
		
		// 렌더링 하는곳
	}
	GraphicsSystemLayer::LayerCamera GraphicsSystemLayer::CreateLayerCamera(SharedPtr<Camera> camera)
	{
		LayerCamera result;
		result.Camera = camera;
		result.Mesh = IMesh::Create(TT("LayerCameraMesh"));
		result.Mesh->SetInputLayout(JGQuadVertex::GetInputLayout());
		
		auto vBuffer = IVertexBuffer::Create(TT("LayerCameraQuad_VBuffer"), EBufferLoadMethod::CPULoad);
		auto iBuffer = IIndexBuffer::Create(TT("LayerCameraQuad_IBuffer"), EBufferLoadMethod::CPULoad);

		JGQuadVertex quadVertex[4];
		u32 quadIndex[6];
		quadVertex[0].Position = JVector3(-0.5f, -0.5f, 0.0f); quadVertex[1].Position = JVector3(-0.5f, +0.5f, 0.0f);
		quadVertex[2].Position = JVector3(+0.5f, +0.5f, 0.0f); quadVertex[3].Position = JVector3(+0.5f, -0.5f, 0.0f);

		quadVertex[0].Texcoord = JVector2(0.0f, 1.0f); quadVertex[1].Texcoord = JVector2(0.0f, 0.0f);
		quadVertex[2].Texcoord = JVector2(1.0f, 0.0f); quadVertex[3].Texcoord = JVector2(1.0f, 1.0f);

		quadIndex[0] = 0; quadIndex[1] = 1; quadIndex[2] = 2; quadIndex[3] = 0; quadIndex[4] = 2; quadIndex[5] = 3;
		vBuffer->SetData(quadVertex, sizeof(JGQuadVertex), 4);
		iBuffer->SetData(quadIndex, 6);

		result.Mesh->AddVertexBuffer(vBuffer);
		result.Mesh->SetIndexBuffer(iBuffer);
		return result;
	}
	EScheduleResult GraphicsSystemLayer::Update()
	{
		if (mIsRenderingReady == false)
		{
			mIsRenderingReady = true;
			NotifyRenderingReadyCompeleteEvent e;
			Application::GetInstance().SendEventImmediate(e);
		}
		else
		{
			
			static int test = 0;
			SortedDictionary<i64, SharedPtr<Camera>> sortedLayerCameraList;
			for (auto& cam : mLayerCameras)
			{
				sortedLayerCameraList[cam.second.Camera->GetDepth()] = cam.second.Camera;
			}
			if (test == 0)
			{
				for (auto& layerCamera : sortedLayerCameraList)
				{
					if (layerCamera.second->IsEnable() == false)
					{
						continue;
					}
					u64 layerMask = layerCamera.second->GetCullingLayerMask();
					if (Renderer::Begin(layerCamera.second) == true)
					{
						for (auto& item : mPushedRenderItemList)
						{
							if (layerMask & GameLayer::GetMask(item->TargetLayer))
							{
								Rendering(layerCamera.second, item);
							}
						}
						Renderer::End();
					}
				}
				test = 1;
			}
			else
			{
				test = 0;
				mIsRenderingReady = false;
				if (Renderer::Begin(mMainCamera) == true)
				{
					mLayerMaterial->SetFloat4x4(TT("gViewProj"), JMatrix::Transpose(mMainCamera->GetViewProjMatrix()));

					for (auto& layerCamera : sortedLayerCameraList)
					{
						if (layerCamera.second->IsEnable() == false)
						{
							continue;
						}
						mLayerMaterial->SetTexture(TT("gTexture"), 0, layerCamera.second->GetTargetTexture());
						Renderer::DrawCall(mLayerCameras[layerCamera.second.get()].Mesh, mLayerMaterial);

					}
					Renderer::End();
				}
				mPushedRenderItemList.clear();
			}
		}
		return EScheduleResult::Continue;
	}
	void GraphicsSystemLayer::LoadShaderScript()
	{
		{
			auto shader = IShader::Create(ShaderScript::Standard2DShader,
				TT(R"(
SamplerState gPointSampler
{
	Template = Point_Wrap
};

Texture2D gTexture;

cbuffer Camera
{
	float4x4 gViewProj;
};
cbuffer Material
{
	float4 gColor;
}
cbuffer Object
{
	float4x4 gWorld;
}

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
	float3 posW = mul(float4(vin.posL, 1.0f), gWorld);
	vout.posH   = mul(float4(posW, 1.0f), gViewProj);
	vout.tex = vin.tex;
	return vout;
}
float4 ps_main(VS_OUT pin) : SV_TARGET
{
	return gTexture.Sample(gPointSampler, pin.tex) * gColor;
}
)"), EShaderFlags::Allow_VertexShader | EShaderFlags::Allow_PixelShader);
			if (shader != nullptr)
			{
				ShaderLibrary::GetInstance().RegisterShader(shader);
			}
			else
			{
				JG_CORE_ERROR("Failed Compile {0}", shader->GetName());
			}
			
		}
		
	}
}

