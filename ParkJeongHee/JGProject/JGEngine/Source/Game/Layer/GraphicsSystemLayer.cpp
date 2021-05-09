#include "pch.h"
#include "GraphicsSystemLayer.h"
#include "Application.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Resource.h"
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
		Scheduler::GetInstance().Schedule(0, 0.08f, -1, SchedulePriority::EndSystem, SCHEDULE_BIND_FN(&GraphicsSystemLayer::Update));

		mMainCamera = Camera::Create(GameSettings::GetResolution(), Math::ConvertToRadians(90), 0.001f, 10000.0f, true);
		mMainCamera->SetLocation(JVector3(0, 0, -10));
		TextureInfo info = {};
		info.ArraySize = 1; info.Flags = ETextureFlags::Allow_RenderTarget;
		info.Format = ETextureFormat::R8G8B8A8_Unorm;
		info.Width = mMainCamera->GetResolution().x;
		info.Height = mMainCamera->GetResolution().y;
		info.MipLevel = 1;
		info.ClearColor = Color::Red();
		auto mainTexture = ITexture::Create(TT("MainTexture"), info);
		mMainCamera->SetTargetTexture(mainTexture);
		mMainCamera->SetCullingLayerMask(JG_U64_MAX);
	}

	void GraphicsSystemLayer::Destroy()
	{

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
		mLayerCameras[e.SharedCamera.get()] = e.SharedCamera;

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
			Renderer::DrawCall(ri->Mesh, ri->Material);
			//Renderer2D::DrawCall(ri->WorldMatrix, ri->Texture, ri->Color);
		}
		else
		{

		}
		
		// 렌더링 하는곳
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
			mIsRenderingReady = false;

			SortedDictionary<i64, SharedPtr<Camera>> sortedLayerCameraList;
			for (auto& cam : mLayerCameras)
			{
				sortedLayerCameraList[cam.second->GetDepth()] = cam.second;
			}

			//for (auto& layerCamera : sortedLayerCameraList)
			//{
			//	if (layerCamera.second->IsEnable() == false)
			//	{
			//		continue;
			//	}
			//	u64 layerMask = layerCamera.second->GetCullingLayerMask();
			//	if (Renderer::Begin(layerCamera.second) == true)
			//	{
			//		for (auto& item : mPushedRenderItemList)
			//		{
			//			if (layerMask & GameLayer::GetMask(item->TargetLayer))
			//			{
			//				Rendering(layerCamera.second, item);
			//			}
			//		}
			//		Renderer::End();
			//	}
			//}

			//for (auto& layerCamera : sortedLayerCameraList)
			//{
			//	if (Renderer::Begin(mMainCamera) == true)
			//	{
			//
			//		Renderer::End();
			//	}
			//}
			
			if (Renderer::Begin(mMainCamera) == true)
			{

				for (auto& item : mPushedRenderItemList)
				{
					Rendering(mMainCamera, item);
				}
				//Renderer2D::DrawCall(JVector2(0, 0), JVector2(100,100), nullptr);
				Renderer::End();
			}
			mPushedRenderItemList.clear();

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
	float4x4 gPadding1;
	float4x4 gPadding2;
	float4x4 gPadding3;
};
cbuffer Material
{
	float4 gColor;
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
	vout.posH = mul(float4(vin.posL, 1.0f), gViewProj);
	vout.tex = vin.tex;
	return vout;
}
float4 ps_main(VS_OUT pin) : SV_TARGET
{
	return gColor;
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

