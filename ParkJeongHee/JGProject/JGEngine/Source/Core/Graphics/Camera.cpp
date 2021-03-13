#include "pch.h"
#include "Camera.h"


namespace JG
{
	void Camera::SetLocation(const JVector3& location)
	{
		mIsViewDirty = true;
		mLocation = location;
	}

	void Camera::SetRotation(const JVector3& rotation)
	{
		mIsViewDirty = true;
		mRotation = JVector3(
			Math::ConvertToRadians(rotation.x),
			Math::ConvertToRadians(rotation.y),
			Math::ConvertToRadians(rotation.z));
	}

	void Camera::SetFOV(float fov)
	{
		mIsProjDirty = true;
		mFov = fov;
	}

	void Camera::SetFarZ(float farZ)
	{
		mIsProjDirty = true;
		mFarZ = farZ;
	}

	void Camera::SetNearZ(float nearZ)
	{
		mIsProjDirty = true;
		mNearZ = nearZ;
	}

	void Camera::SetOrthographic(bool isOrthographic)
	{
		mIsProjDirty = true;
		mIsOrthographic = isOrthographic;
	}

	void Camera::SetResolution(const JVector2& resolution)
	{
		mIsProjDirty = true;
		mResolution = resolution;
	}

	const JVector3& Camera::GetLocation() const
	{
		return mLocation;
	}

	const JVector3& Camera::GetRotation() const
	{
		return mRotation;
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

	float Camera::GetFOV() const
	{
		return mFov;
	}

	float Camera::GetFarZ() const
	{
		return mFarZ;
	}

	float Camera::GetNearZ() const
	{
		return mNearZ;
	}

	JVector3 Camera::GetLook() const
	{
		return JVector3::Normalize(JMatrix::Rotation(mRotation).TransformVector(JVector3(0, 0, 1)));
	}

	JVector3 Camera::GetRight() const
	{
		return JVector3::Normalize(JMatrix::Rotation(mRotation).TransformVector(JVector3(1, 0, 0)));
	}

	JVector3 Camera::GetUp() const
	{
		return JVector3::Normalize(JMatrix::Rotation(mRotation).TransformVector(JVector3(0, 1, 0)));
	}

	float Camera::GetAspectRatio() const
	{
		return mResolution.x / mResolution.y;
	}

	const JVector2& Camera::GetResolution() const
	{
		return mResolution;
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
		target = JMatrix::Rotation(mRotation).TransformVector(target);
		mViewMatrix = JMatrix::LookAtLH(mLocation, mLocation + target, JVector3(0, 1, 0));

		return true;
	}


	SharedPtr<Camera> Camera::Create(const JVector2& resolution, float fov, float nearZ, float farZ, bool isOrthographic)
	{
		SharedPtr<Camera> camera = CreateSharedPtr<Camera>();


		camera->SetResolution(resolution);
		camera->SetFOV(fov);
		camera->SetFarZ(farZ);
		camera->SetNearZ(nearZ);
		camera->SetOrthographic(isOrthographic);



		return camera;
	}

}