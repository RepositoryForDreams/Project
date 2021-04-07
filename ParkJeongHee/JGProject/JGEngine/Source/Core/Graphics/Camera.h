#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"






namespace JG
{
	class ITexture;
	class Camera
	{
	private:
		mutable bool mIsViewDirty    = true;
		mutable bool mIsProjDirty    = true;
	
	
		mutable JMatrix  mProjMatrix;
		mutable JMatrix  mViewMatrix;
		mutable JMatrix  mViewProjMatrix;

		JVector2 mResolution;
		JVector3 mLocation;
		JVector3 mRotation;
		
		float mNearZ = 0.0f;
		float mFarZ  = 0.0f;
		float mFov   = 0.0f;
		bool mIsOrthographic = false;
		//
		List<SharedPtr<ITexture>> mTargetTextures;
		SharedPtr<ITexture> mTargetDepthTexture;
	public:
		Camera();
	public:
		void SetLocation(const JVector3& location);
		void SetRotation(const JVector3& rotation);

		void SetFOV(float fov);
		void SetFarZ(float farZ);
		void SetNearZ(float nearZ);

		void SetOrthographic(bool isOrthographic);
		void SetResolution(const JVector2& resolution);

		bool SetTargetTexture(SharedPtr<ITexture> texture, u8 slot = 0);
		bool SetTargetDepthTexture(SharedPtr<ITexture> texture);
	public:
		const JVector3& GetLocation() const;
		const JVector3& GetRotation() const;
		const JMatrix& GetViewProjMatrix() const;
		const JMatrix& GetViewMatrix() const;
		const JMatrix& GetProjMatrix() const;

		float GetFOV() const;
		float GetFarZ() const;
		float GetNearZ() const;

		JVector3 GetLook()  const;
		JVector3 GetRight() const;
		JVector3 GetUp()    const;

		bool IsOrthographic() const;
		float GetAspectRatio() const;
		const JVector2& GetResolution() const;
		SharedPtr<ITexture> GetTargetTexture(u8 slot = 0) const;
		const List<SharedPtr<ITexture>>& GetTargetTextures() const;
		SharedPtr<ITexture> GetTargetDepthTexture() const;
	private:
		bool UpdateProj() const;
		bool UpdateView() const;
	public:
		static SharedPtr<Camera> Create(const JVector2& resolution, float fov, float nearZ, float farZ, bool isOrthographic);
	};
}

