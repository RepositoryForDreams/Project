#include "pch.h"
#include "GraphicsSystemLayer.h"
#include "Application.h"
#include "Graphics/GraphicsDefine.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Resource.h"
#include "Graphics/GraphicsAPI.h"
#include "Class/Game/GameSettings.h"
#include "Class/Game/Components/Camera.h"



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

		mMainCamera = MainCamera(GameSettings::GetResolution());
		mRenderer2D = CreateSharedPtr<Renderer2D>();


		mRenderItemPriority[JGTYPE(Standard2DRenderItem)] = (u64)ERenderItemPriority::_2D;
	}

	void GraphicsSystemLayer::Destroy()
	{
		mMainCamera.Clear();
		mRenderer2D.reset();
		mRegisteredCameras.clear();
		mPushedRenderItems.clear();
		
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
		auto type     = e.RenderItem->GetType();
		auto priority = mRenderItemPriority[type];
		mPushedRenderItems[(u64)priority][type].push_back(e.RenderItem);

		return true;
	}

	bool GraphicsSystemLayer::ResponseGetMainSceneTexture(RequestGetMainSceneTextureEvent& e)
	{
		e.SceneTexture = mMainCamera.GetTargetTexture();


		return true;
	}

	bool GraphicsSystemLayer::ResponseRegisterCamera(RequestRegisterCameraEvent& e)
	{
		if (mRegisteredCameras.find(e.SharedCamera) != mRegisteredCameras.end())
		{
			return true;
		}
		mRegisteredCameras[e.SharedCamera] = CreateLayerCamera(e.SharedCamera);

		return true;
	}

	bool GraphicsSystemLayer::ResponseUnRegisterCamera(RequestUnRegisterCameraEvent& e)
	{
		if (mRegisteredCameras.find(e.SharedCamera) == mRegisteredCameras.end())
		{
			return true;
		}

		mRegisteredCameras.erase(e.SharedCamera);
		return true;
	}

	void GraphicsSystemLayer::Rendering(const CameraItem& cameraItem, Type type, const List<SharedPtr<IRenderItem>>& renderItemList)
	{
		if (type == JGTYPE(Standard2DRenderItem))
		{
			u64 layerMask = cameraItem.Camera->GetCullingLayerMask();
			if (cameraItem.Renderer2D->Begin(
				cameraItem.Camera->GetResolution(), cameraItem.Camera->GetViewProjMatrix(), cameraItem.Camera->GetTargetTexture()) == true)
			{
				for (auto& item : renderItemList)
				{
					if (layerMask & GameLayer::GetMask(item->TargetLayer))
					{
						auto _2dItem = static_cast<Standard2DRenderItem*>(item.get());
						cameraItem.Renderer2D->DrawCall(_2dItem->WorldMatrix, _2dItem->Texture, _2dItem->Color);
					}
				}
				cameraItem.Renderer2D->End();
			}
		}
		else
		{
			// Not Supported
		}
		
		// 렌더링 하는곳
	}
	GraphicsSystemLayer::CameraItem GraphicsSystemLayer::CreateLayerCamera(Camera* camera)
	{
		CameraItem result;
		result.Camera = camera;
		result.Renderer2D = CreateSharedPtr<Renderer2D>();
		return result;
	}
	EScheduleResult GraphicsSystemLayer::Update()
	{
		switch (mRenderingState)
		{
		case ERenderingState::Wait:
			mRenderingState = ERenderingState::ReadyCompelete;
			return EScheduleResult::Continue;
		case ERenderingState::ReadyCompelete:
		{
			mSortedLayerCameraList.clear();
			mPushedRenderItems.clear();

			NotifyRenderingReadyCompeleteEvent e;
			Application::GetInstance().SendEventImmediate(e);
	
		}
		mRenderingState = ERenderingState::RenderRegisteredCamera;
		return EScheduleResult::Continue;
		case ERenderingState::RenderRegisteredCamera:
		{
			for (auto& cam : mRegisteredCameras)
			{
				mSortedLayerCameraList[cam.second.Camera->GetDepth()] = cam.second;
			}
			for (auto& layerCamera : mSortedLayerCameraList)
			{
				if (layerCamera.second.Camera->IsActive() == false)
				{
					continue;
				}
				for (auto& _first_pair : mPushedRenderItems)
				{
					for (auto& _second_pair : _first_pair.second)
					{
						auto type = _second_pair.first;
						auto& renderItemList = _second_pair.second;
						Rendering(layerCamera.second, type, renderItemList);
					}
				}
			}

		}

		mRenderingState = ERenderingState::RenderMainCamera;
		return EScheduleResult::Continue;
		case ERenderingState::RenderMainCamera:
		{
			if (mRenderer2D->Begin(mMainCamera.GetResolution(), mMainCamera.GetViewProj(), mMainCamera.GetTargetTexture()))
			{
				for (auto& layerCamera : mSortedLayerCameraList)
				{
					mRenderer2D->DrawCall(JVector2(0, 0), layerCamera.second.Camera->GetResolution(), layerCamera.second.Camera->GetTargetTexture());
				}
				mRenderer2D->End();
			}
		}
			mRenderingState = ERenderingState::ReadyCompelete;
			return EScheduleResult::Continue;
		default:
			return EScheduleResult::Continue;
		}
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
			ShaderLibrary::RegisterShader(shader);
			
		}
		
	}
}

