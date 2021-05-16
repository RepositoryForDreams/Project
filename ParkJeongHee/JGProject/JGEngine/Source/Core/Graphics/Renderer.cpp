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
}