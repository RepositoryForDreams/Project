#include "pch.h"
#include "GameCamera.h"
#include "Transform.h"

#include "Graphics/Resource.h"
namespace JG
{
	void GameCamera::Awake()
	{
		mCamera = Camera::Create(GameSettings::GetResolution(),
			90, 0.001f, 10000.0f, false);

		TextureInfo info = {};
		info.ArraySize = 1; info.Flags = ETextureFlags::Allow_RenderTarget;
		info.Format = ETextureFormat::R8G8B8A8_Unorm;
		info.Width = mCamera->GetResolution().x;
		info.Height = mCamera->GetResolution().y;
		info.MipLevel = 1;
		info.ClearColor = Color::White();
		auto texture = ITexture::Create(GetName() + TT("_CameraTexture"), info);
		mCamera->SetTargetTexture(texture);
	}
	void GameCamera::Start()
	{
		RequestRegisterCameraEvent e;
		e.SharedCamera = mCamera;
		SendEvent(e);
	}
	void GameCamera::Destory()
	{
		RequestUnRegisterCameraEvent e;
		e.SharedCamera = mCamera;
		SendEvent(e);
	}
	void GameCamera::OnChange(const ChangeData& data)
	{
		if (data.Type == JGTYPE(Transform))
		{
			auto transform = static_cast<Transform*>(data.Object);
			mCamera->SetLocation(transform->GetWorldLocation());
			mCamera->SetRotation(transform->GetWorldRotation());
		}
	}
	void GameCamera::OnInspectorGUI()
	{
		f32 fov    = mCamera->GetFOV();
		i64 depth  = mCamera->GetDepth();
		f32 nearZ  = mCamera->GetNearZ();
		f32 farZ   = mCamera->GetFarZ();
		bool isOrth = mCamera->IsOrthographic();



		ImGui::OnGui("Field of View", &fov);
		ImGui::OnGui("Depth", &depth);
		ImGui::OnGui("NearZ", &nearZ);
		ImGui::OnGui("FarZ", &farZ);

		ImGui::Checkbox("Orthographic", &isOrth);


		mCamera->SetFOV(fov);
		mCamera->SetDepth(depth);
		mCamera->SetFarZ(farZ);
		mCamera->SetNearZ(nearZ);
		mCamera->SetOrthographic(isOrth);
	}
}