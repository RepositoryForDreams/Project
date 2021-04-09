#include "pch.h"
#include "GraphicsLayer.h"
#include "Application.h"
#include "Graphics/Renderer.h"
#include "Class/Asset/AssetImporter.h"
#include "UI/UIView/SceneView.h"
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

		TextureInfo textureInfo;

		textureInfo.Width = 800;
		textureInfo.Height = 600;
		textureInfo.ClearColor = Color::Green();
		textureInfo.Format = ETextureFormat::R8G8B8A8_Unorm;
		textureInfo.MipLevel = 1;
		textureInfo.Flags = ETextureFlags::Allow_RenderTarget;
		textureInfo.ArraySize = 1;
		auto texture = ITexture::Create(TT("Test_Texture"), textureInfo);
		mCamera->SetTargetTexture(texture);


		


		auto meshPath = CombinePath(Application::GetAssetPath(), TT("Samples/Model/Sphere.mesh"));
		mMesh = IMesh::CreateFromFile(meshPath);


		Scheduler::GetInstance().Schedule(0, 0.16f, -1, SchedulePriority::Default, SCHEDULE_BIND_FN(&GraphicsLayer::Update));

		Scheduler::GetInstance().Schedule(0, 0.16f, -1, SchedulePriority::Default, 
			[&]() -> EScheduleResult
		{
			auto sceneView = UIManager::GetInstance().GetUIView<SceneView>();
			if (sceneView != nullptr)
			{
				auto sceneVm = sceneView->GetViewModel();
				if (sceneVm != nullptr)
				{
					sceneVm->SetSceneTexture(mCamera->GetTargetTexture());
					return EScheduleResult::Break;
				}
			}
			return EScheduleResult::Continue;
		});
		
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


			Renderer3D::End();
		}


		return EScheduleResult::Continue;
	}
}

