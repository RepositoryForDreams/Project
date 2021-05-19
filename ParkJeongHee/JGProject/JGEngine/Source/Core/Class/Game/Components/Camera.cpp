#include "Camera.h"
#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Class/Game/GameNode.h"

#include "Graphics/Resource.h"
namespace JG
{
	void Camera::Awake()
	{
		mTargetTextures.resize(MAX_RENDERTARGET, nullptr);


		//mCamera = Camera::Create(GameSettings::GetResolution(),
		//	90, 0.001f, 10000.0f, false);
		SetFOV(90);
		SetNearZ(0.001f);
		SetFarZ(10000.0f);
		SetOrthographic(true);
		SetResolution(GameSettings::GetResolution());
	}

	void Camera::Start()
	{
		RequestRegisterCameraEvent e;
		e.SharedCamera = this;
		SendEvent(e);
	}
	void Camera::Destory()
	{
		RequestUnRegisterCameraEvent e;
		e.SharedCamera = this;
		SendEvent(e);
	}
	void Camera::Serialize(FileStreamWriter* writer) const
	{
		GameComponent::Serialize(writer);
		writer->Write(GetResolution());
		writer->Write(GetNearZ());
		writer->Write(GetFarZ());
		writer->Write(GetFOV());
		writer->Write(GetDepth());
		writer->Write(GetCullingLayerMask());
		writer->Write(IsOrthographic());
	}
	void Camera::DeSerialize(FileStreamReader* reader)
	{
		JVector2 resolution;
		f32 nearZ = 0.0f;
		f32 farZ = 0.0f;
		f32 fov = 0.0f;
		i64 depth = 0;
		u64 cullingLayerMask = JG_U64_MAX;
		bool isOrthographic = false;

		GameComponent::DeSerialize(reader);
		reader->Read(&resolution);
		reader->Read(&nearZ);
		reader->Read(&farZ);
		reader->Read(&fov);
		reader->Read(&depth);
		reader->Read(&cullingLayerMask);
		reader->Read(&isOrthographic);

		SetResolution(resolution);
		SetNearZ(nearZ);
		SetFarZ(farZ);
		SetFOV(fov);
		SetDepth(depth);
		SetCullingLayerMask(cullingLayerMask);
		SetOrthographic(isOrthographic);
	}
	void Camera::SetFOV(f32 fov)
	{
		f32 convertFOV = Math::ConvertToRadians(fov);
		if (mFov != convertFOV)
		{
			mIsProjDirty = true;
		}
		mFov = convertFOV;
	}

	void Camera::SetFarZ(f32 farZ)
	{
		if (mFarZ != farZ)
		{
			mIsProjDirty = true;
		}
		mFarZ = farZ;
	}

	void Camera::SetNearZ(f32 nearZ)
	{
		if (mNearZ != nearZ)
		{
			mIsProjDirty = true;
		}
		mNearZ = nearZ;
	}

	void Camera::SetOrthographic(bool isOrthographic)
	{
		if (mIsOrthographic == isOrthographic)
		{
			return;
		}
		mIsProjDirty = true;
		mIsOrthographic = isOrthographic;
	}

	void Camera::SetResolution(const JVector2& resolution)
	{
		if (mResolution == resolution)
		{
			return;
		}
		mIsProjDirty = true;
		mResolution = resolution;

		// CreateTexture;
		TextureInfo info = {};
		info.ArraySize = 1; info.Flags = ETextureFlags::Allow_RenderTarget;
		info.Format = ETextureFormat::R8G8B8A8_Unorm;
		info.Width  = mResolution.x;
		info.Height = mResolution.y;
		info.MipLevel = 1;
		mTargetTextures[0] = (ITexture::Create(GetName() + TT("_CameraTexture"), info));



		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		api->ClearTexture(mTargetTextures[0]);
	}
	void Camera::SetCullingLayerMask(u64 mask)
	{
		mCullingLayerMask = mask;
	}
	void Camera::SetDepth(i64 depth)
	{
		mDepth = depth;
	}
	const JMatrix& Camera::GetViewProjMatrix() const
	{
		bool isUpdateView = UpdateView();
		bool isUpdateProj = UpdateProj();
		if (isUpdateView || isUpdateProj)
		{
			mViewProjMatrix = mViewMatrix * mProjMatrix;
		}
		return mViewProjMatrix;
	}

	const JMatrix& Camera::GetViewMatrix() const
	{
		UpdateView();

		return mViewMatrix;
	}

	const JMatrix& Camera::GetProjMatrix() const
	{
		UpdateProj();

		return mProjMatrix;
	}

	f32 Camera::GetFOV() const
	{
		return Math::ConvertToDegrees(mFov);
	}

	f32 Camera::GetFarZ() const
	{
		return mFarZ;
	}

	f32 Camera::GetNearZ() const
	{
		return mNearZ;
	}

	JVector3 Camera::GetLook() const
	{
		auto rotation = GetOwner()->GetTransform()->GetWorldRotation();
		rotation = JVector3(Math::ConvertToDegrees(rotation.x), Math::ConvertToDegrees(rotation.y), Math::ConvertToDegrees(rotation.z));
		return JVector3::Normalize(JMatrix::Rotation(rotation).TransformVector(JVector3(0, 0, 1)));
	}

	JVector3 Camera::GetRight() const
	{
		auto rotation = GetOwner()->GetTransform()->GetWorldRotation();
		rotation = JVector3(Math::ConvertToDegrees(rotation.x), Math::ConvertToDegrees(rotation.y), Math::ConvertToDegrees(rotation.z));
		return JVector3::Normalize(JMatrix::Rotation(rotation).TransformVector(JVector3(1, 0, 0)));
	}

	JVector3 Camera::GetUp() const
	{
		auto rotation = GetOwner()->GetTransform()->GetWorldRotation();
		rotation = JVector3(Math::ConvertToDegrees(rotation.x), Math::ConvertToDegrees(rotation.y), Math::ConvertToDegrees(rotation.z));
		return JVector3::Normalize(JMatrix::Rotation(rotation).TransformVector(JVector3(0, 1, 0)));
	}

	bool Camera::IsOrthographic() const
	{
		return mIsOrthographic;
	}

	float Camera::GetAspectRatio() const
	{
		return mResolution.x / mResolution.y;
	}

	const JVector2& Camera::GetResolution() const
	{
		return mResolution;
	}
	SharedPtr<ITexture> Camera::GetTargetTexture(u8 slot) const
	{
		if (slot >= MAX_RENDERTARGET)
		{
			return nullptr;
		}
		return mTargetTextures[slot];
	}
	const List<SharedPtr<ITexture>>& Camera::GetTargetTextures() const
	{
		return mTargetTextures;
	}
	SharedPtr<ITexture> Camera::GetTargetDepthTexture() const
	{
		return mTargetDepthTexture;
	}
	u64 Camera::GetCullingLayerMask() const
	{
		return mCullingLayerMask;
	}
	i64 Camera::GetDepth() const
	{
		return mDepth;
	}
	bool Camera::UpdateProj() const
	{
		if (mIsProjDirty == false)
		{
			return false;
		}
		mIsProjDirty = false;


		if (mIsOrthographic)
		{
			mProjMatrix = JMatrix::OrthographicLH(mResolution.x, mResolution.y, mNearZ, mFarZ);
		}
		else
		{
			mProjMatrix = JMatrix::PerspectiveFovLH(mFov, GetAspectRatio(), mNearZ, mFarZ);
		}

		return true;
	}

	bool Camera::UpdateView() const
	{
		if (mIsViewDirty == false)
		{
			return false;
		}
		mIsViewDirty = false;



		JVector3 target = JVector3(0, 0, 1);
		auto rotation = GetOwner()->GetTransform()->GetWorldRotation();
		rotation = JVector3(Math::ConvertToDegrees(rotation.x), Math::ConvertToDegrees(rotation.y), Math::ConvertToDegrees(rotation.z));
		auto& location = GetOwner()->GetTransform()->GetWorldLocation();
		location.z -= GetNearZ();
		target = JMatrix::Rotation(rotation).TransformVector(target);
		mViewMatrix = JMatrix::LookAtLH(location, location + target, JVector3(0, 1, 0));

		return true;
	}
	void Camera::OnChange(const ChangeData& data)
	{
		if (data.Type == JGTYPE(Transform))
		{
			mIsViewDirty = true;
			//auto transform = static_cast<Transform*>(data.Object);
			//mCamera->SetLocation(transform->GetWorldLocation());
			//mCamera->SetRotation(transform->GetWorldRotation());
		}
	}
	void Camera::OnInspectorGUI()
	{
		f32 fov = mFov;
		i64 depth = mDepth;
		f32 nearZ = mNearZ;
		f32 farZ = mFarZ;
		bool isOrth = mIsOrthographic;



		ImGui::OnGui("Field of View", &fov);
		ImGui::OnGui("Depth", &depth);
		ImGui::OnGui("NearZ", &nearZ);
		ImGui::OnGui("FarZ", &farZ);

		ImGui::Checkbox("Orthographic", &isOrth);


		SetFOV(fov);
		SetDepth(depth);
		SetFarZ(farZ);
		SetNearZ(nearZ);
		SetOrthographic(isOrth);
	}
}