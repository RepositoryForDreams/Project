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
#include "Class/Asset/AssetImporter.h"

#include "Imgui/ImGuizmo.h"


namespace JG
{
	GraphicsSystemLayer::CameraItem::CameraItem(Camera* cam) : pCamera(cam)
	{
		auto bufferCnt = Application::GetInstance().GetGraphicsAPI()->GetBufferCount();
		TargetTextures.resize(bufferCnt, nullptr);
		TargetDepthTextures.resize(bufferCnt, nullptr);
		CurrentIndex = 0;

		_2DBatch = CreateSharedPtr<Render2DBatch>();
	}

	bool GraphicsSystemLayer::CameraItem::IsValid() const
	{
		return TargetTextures[0] != nullptr;
	}

	void GraphicsSystemLayer::CameraItem::ChangeRenderer()
	{
		if (pCamera == nullptr) return;
		if (Renderer != nullptr && pCamera->GetRendererPath() == Renderer->GetRendererPath())
		{
			return;
		}

		auto bufferCnt = Application::GetInstance().GetGraphicsAPI()->GetBufferCount();

		
		if (Renderer != nullptr)
		{
			bool isMakeSchedule = PendingRendererQueue.empty();
			PendingRendererQueue.push(Renderer);

			if (isMakeSchedule)
			{
				Scheduler::GetInstance().ScheduleOnceByFrame(bufferCnt, 0,
					[&]() -> EScheduleResult
				{

					while (!PendingRendererQueue.empty())
					{
						PendingRendererQueue.front().reset();
						PendingRendererQueue.pop();
					}
					return EScheduleResult::Continue;
				});
			}
		}
	

		switch (pCamera->GetRendererPath())
		{
		case ERendererPath::Foward:
			Renderer = CreateSharedPtr<FowardRenderer>();
			break;
		case ERendererPath::Deferred:
			break;
		}

	}


	void GraphicsSystemLayer::OnAttach()
	{
		LoadShaderScript();
	}

	void GraphicsSystemLayer::OnDetach()
	{



	}

	void GraphicsSystemLayer::Begin()
	{
		Scheduler::GetInstance().Schedule(0, 0.0f, -1, SchedulePriority::EndSystem, SCHEDULE_BIND_FN(&GraphicsSystemLayer::Update));
		mMainCamera = CreateUniquePtr<CameraItem>();


		mRenderItemPriority[JGTYPE(Standard2DRenderItem)] = (u64)ERenderItemPriority::_2D;
		mStandardDefaultMaterial = IMaterial::Create("DefaultMaterial", ShaderLibrary::Get(ShaderScript::Standard3DShader));


		// юс╫ц
		String rawAssetPath = CombinePath(Application::GetAssetPath(), "RawResources");
		String outputPath = CombinePath(Application::GetAssetPath(), "Resources");

		for (auto& iter : fs::recursive_directory_iterator(rawAssetPath))
		{
			auto extenstion = iter.path().extension().string();
			if (extenstion == ".fbx")
			{
				FBXAssetImportSettings settings;
				settings.AssetPath = iter.path().string();
				settings.OutputPath = outputPath;
				auto result = AssetImporter::Import(settings);
				if (result == EAssetImportResult::Success)
				{
					JG_CORE_INFO("Success Import {0}", iter.path().string());
				}
				else
				{
					JG_CORE_INFO("Fail Import {0}", iter.path().string());
				}
			}
			if (extenstion == ".png" || extenstion == ".jpg" || extenstion == ".TGA")
			{
				TextureAssetImportSettings settings;
				settings.AssetPath = iter.path().string();
				settings.OutputPath = outputPath;
				auto result = AssetImporter::Import(settings);
				if (result == EAssetImportResult::Success)
				{
					JG_CORE_INFO("Success Import {0}", iter.path().string());
				}
				else
				{
					JG_CORE_INFO("Fail Import {0}", iter.path().string());
				}
			}

		}
		for (auto& iter : fs::recursive_directory_iterator(outputPath))
		{
			//AssetDataBase::GetInstance().LoadOriginAsset(OUTPUT)
		}
	}

	void GraphicsSystemLayer::Destroy()
	{
		mRegisteredRenderCameras.clear();
		mPushedRenderItems.clear();
	}

	void GraphicsSystemLayer::OnEvent(IEvent& e)
	{
		EventDispatcher eventDispatcher(e);
		eventDispatcher.Dispatch<RequestPushRenderItemEvent>(EVENT_BIND_FN(&GraphicsSystemLayer::ResponsePushRenderItem));
		eventDispatcher.Dispatch<RequestRegisterCameraEvent>(EVENT_BIND_FN(&GraphicsSystemLayer::ResponseRegisterCamera));
		eventDispatcher.Dispatch<RequestUnRegisterCameraEvent>(EVENT_BIND_FN(&GraphicsSystemLayer::ResponseUnRegisterCamera));
		eventDispatcher.Dispatch<RequestRegisterMainCameraEvent>(EVENT_BIND_FN(&GraphicsSystemLayer::ResponseRegisterMainCamera));
		eventDispatcher.Dispatch<RequestUnRegisterMainCameraEvent>(EVENT_BIND_FN(&GraphicsSystemLayer::ResponseUnRegisterMainCamera));
	}

	String GraphicsSystemLayer::GetLayerName()
	{
		return "GraphicsSystemLayer";
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


	bool GraphicsSystemLayer::ResponseRegisterCamera(RequestRegisterCameraEvent& e)
	{
		if (mRegisteredRenderCameras.find(e.pCamera) != mRegisteredRenderCameras.end())
		{
			return true;
		}
		
		mRegisteredRenderCameras[e.pCamera] = CreateUniquePtr<CameraItem>(e.pCamera);

		return true;
	}

	bool GraphicsSystemLayer::ResponseUnRegisterCamera(RequestUnRegisterCameraEvent& e)
	{
		if (mRegisteredRenderCameras.find(e.pCamera) == mRegisteredRenderCameras.end())
		{
			return true;
		}

		mRegisteredRenderCameras.erase(e.pCamera);
		return true;
	}

	bool GraphicsSystemLayer::ResponseRegisterMainCamera(RequestRegisterMainCameraEvent& e)
	{
		if (mMainCamera->pCamera == nullptr)
		{
			mMainCamera = CreateUniquePtr<CameraItem>(e.MainCamera);
		}
		else
		{
			mMainCamera->pCamera = e.MainCamera;
		}
		return true;
	}

	bool GraphicsSystemLayer::ResponseUnRegisterMainCamera(RequestUnRegisterMainCameraEvent& e)
	{
		if (mMainCamera->pCamera == e.MainCamera)
		{
			mMainCamera->pCamera = nullptr;
		}
		return true;
	}

	void GraphicsSystemLayer::Rendering(CameraItem* cameraItem, Type type, const List<SharedPtr<IRenderItem>>& renderItemList)
	{
		if (type == JGTYPE(Standard2DRenderItem))
		{
			u64 layerMask = cameraItem->pCamera->GetCullingLayerMask();
			
			for (auto& item : renderItemList)
			{
				auto itemMask = GameLayer::GetMask(item->TargetLayer);
				if ((itemMask & layerMask) == false) continue;

				auto _2dItem = static_cast<Standard2DRenderItem*>(item.get());
				cameraItem->_2DBatch->DrawCall(_2dItem->WorldMatrix, _2dItem->Texture, _2dItem->Color);
			}
		}
		if (type == JGTYPE(StandardStaticMeshRenderItem))
		{

			u64 layerMask = cameraItem->pCamera->GetCullingLayerMask();
			for (auto& item : renderItemList)
			{
				auto itemMask = GameLayer::GetMask(item->TargetLayer);
				if ((itemMask & layerMask) == false) continue;

				auto _3dItem = static_cast<StandardStaticMeshRenderItem*>(item.get());
				if (_3dItem->Materials.empty())
				{
					_3dItem->Materials.push_back(mStandardDefaultMaterial);
				}
				cameraItem->Renderer->DrawCall(_3dItem->WorldMatrix, _3dItem->Mesh, _3dItem->Materials);
			}
		}
		else
		{
			// Not Supported
		}
	}
	EScheduleResult GraphicsSystemLayer::Update()
	{
		{
			NotifyRenderingReadyCompeleteEvent e;
			Application::GetInstance().SendEvent(e);
		}
		bool isResize = false;
		if (mMainCamera->pCamera != nullptr && mMainCamera->Resolution != mMainCamera->pCamera->GetResolution())
		{
			isResize = true;
			mMainCamera->IsResizeDirty = true;
			mMainCamera->Resolution = mMainCamera->pCamera->GetResolution();
		}
		for (auto& _pair : mRegisteredRenderCameras)
		{
			auto& camItem = _pair.second;
			if (camItem->Resolution != camItem->pCamera->GetResolution())
			{
				isResize = true;
				camItem->IsResizeDirty = true;
				camItem->Resolution = camItem->pCamera->GetResolution();
			}
		}
		if (isResize == true)
		{
			JG_CORE_INFO("Graphics API Flush");
			Application::GetInstance().GetGraphicsAPI()->Flush();
		}

		auto fmBufferCnt = Application::GetInstance().GetGraphicsAPI()->GetBufferCount();
		if (mMainCamera->pCamera != nullptr)
		{
			auto mainCam = mMainCamera->pCamera;
			if (mMainCamera->IsValid() == false)
			{
				TextureInfo mainTexInfo;
				mainTexInfo.Width  = std::max<u32>(1, mMainCamera->Resolution.x);
				mainTexInfo.Height = std::max<u32>(1, mMainCamera->Resolution.y);
				mainTexInfo.ArraySize = 1;
				mainTexInfo.Format = ETextureFormat::R8G8B8A8_Unorm;
				mainTexInfo.Flags = ETextureFlags::Allow_RenderTarget;
				mainTexInfo.MipLevel = 1;
				mainTexInfo.ClearColor = mainCam->GetClearColor();

				for (auto& t : mMainCamera->TargetTextures)
				{
					t = ITexture::Create(mainCam->GetName() + "TargetTexture", mainTexInfo);
				}

				mainTexInfo.Format = ETextureFormat::D24_Unorm_S8_Uint;
				mainTexInfo.Flags  = ETextureFlags::Allow_DepthStencil;
				for (auto& t : mMainCamera->TargetDepthTextures)
				{
					t = ITexture::Create(mainCam->GetName() + "TargetDepthTexture", mainTexInfo);
				}
			}

			RenderInfo info;
			info.CurrentBufferIndex = mMainCamera->CurrentIndex;
			info.Resolutoin			= mMainCamera->pCamera->GetResolution();
			info.ViewProj = mMainCamera->pCamera->GetViewProjMatrix();
			info.TargetTexture		= mMainCamera->TargetTextures[info.CurrentBufferIndex];
			info.TargetDepthTexture = mMainCamera->TargetDepthTextures[info.CurrentBufferIndex];
			info.TargetTexture->SetClearColor(mainCam->GetClearColor());
			mMainCamera->ChangeRenderer();
			
			if (mMainCamera->Renderer->Begin(info, {mMainCamera->_2DBatch}) == true)
			{
				for (auto& _pair : mPushedRenderItems)
				{
					for (auto& itemPair : _pair.second)
					{
						auto& type = itemPair.first;
						auto& itemList = itemPair.second;
						Rendering(mMainCamera.get(), type, itemList);

						itemList.clear();
					}
				}
				mMainCamera->Renderer->End();
			}

			

			mMainCamera->CurrentIndex = (mMainCamera->CurrentIndex + 1) % fmBufferCnt;
			{
				NotifyChangeMainSceneTextureEvent e;
				e.SceneTexture = mMainCamera->TargetTextures[mMainCamera->CurrentIndex];
				Application::GetInstance().SendEvent(e);
			}

			mPushedRenderItems.clear();
			
		}
		return EScheduleResult::Continue;
	}
	void GraphicsSystemLayer::LoadShaderScript()
	{
		{
			auto shader = IShader::Create(ShaderScript::Standard2DShader,
				R"(
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
		)", EShaderFlags::Allow_VertexShader | EShaderFlags::Allow_PixelShader);
			ShaderLibrary::RegisterShader(shader);
			
		}
		{
			auto shader = IShader::Create(ShaderScript::Standard3DShader,
				R"(
		SamplerState gPointSampler
		{
			Template = Point_Wrap
		};

		cbuffer Camera
		{
			float4x4 gViewProj;
		};
		cbuffer ObjectParams
		{
			float4x4 gWorld;
		}

		struct VS_IN
		{
			float3 posL : POSITION;
			float2 tex : TEXCOORD;
			float3 normalL : NORMAL;
			float3 tanL : TANGENT;
			float3 bitL : BITANGENT;
		};
		struct VS_OUT
		{
			float4 posH    : SV_POSITION;
			float3 posW    : POSITION;
			float3 normalW : NORMAL;
			float2 tex     : TEXCOORD;
		};
		struct PS_MATERIAL_OUTPUT
		{
			float4 albedo;
		};
		struct PS_MATERIAL_INPUT
		{

		};

		PS_MATERIAL_OUTPUT PS_MATERIAL_FUNCTION(PS_MATERIAL_INPUT _input)
		{
			PS_MATERIAL_OUTPUT _output;
			_output.albedo = float4(1.0f,1.0f,1.0f,1.0f);


			//PS_MATERIAL_FUNCTION_SCRIPT
			return _output;
		};
		
		
		VS_OUT vs_main(VS_IN vin)
		{
			VS_OUT vout;
		    
			float3 posW = mul(float4(vin.posL, 1.0f), gWorld);
			float3 normalW = mul(float4(vin.normalL, 1.0f), gWorld);
			float3 tanW =  mul(float4(vin.tanL, 1.0f), gWorld);
			float3 bitW =  mul(float4(vin.bitL, 1.0f), gWorld);
			//vout.posH = mul(float4(vin.posL, 1.0f), gViewProj);
			vout.posH = mul(float4(posW, 1.0f), gViewProj);
			vout.posW = posW;
			vout.normalW = normalize(normalW);
			vout.tex   = vin.tex;
			return vout;
		}
		float4 ps_main(VS_OUT pin) : SV_TARGET
		{
			PS_MATERIAL_INPUT input;
			PS_MATERIAL_OUTPUT output = PS_MATERIAL_FUNCTION(input);

			float3 dirLightColor = float3(0.9f, 0.95f, 1.0f);
			float3 dirLight = float3(0.0f, -1.0f, 1.0f);

			float4 ambientLight = float4(0.2f, 0.2f, 0.25f, 1.0f);

			float3 N = normalize(pin.normalW);
			float3 L = normalize(-dirLight);
			float NdotL = saturate(dot(N,L));


			return saturate(output.albedo * NdotL + ambientLight);
		}
		)", EShaderFlags::Allow_VertexShader | EShaderFlags::Allow_PixelShader);
			ShaderLibrary::RegisterShader(shader);
		}
	}

}

