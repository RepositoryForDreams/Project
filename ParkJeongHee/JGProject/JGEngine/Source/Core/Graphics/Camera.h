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
		
		f32 mNearZ = 0.0f;
		f32 mFarZ  = 0.0f;
		f32 mFov   = 0.0f;

		i64 mDepth = 0;
		u64 mCullingLayerMask = JG_U64_MAX;
		String mTargetLayer   = GameLayer::DEFAULT_LAYER;

		bool mIsOrthographic = false;
		bool mEnable = true;
		//
		List<SharedPtr<ITexture>> mTargetTextures;
		SharedPtr<ITexture> mTargetDepthTexture;
	public:
		Camera();
	public:
		void SetLocation(const JVector3& location);
		void SetRotation(const JVector3& rotation);

		void SetFOV(f32 fov);
		void SetFarZ(f32 farZ);
		void SetNearZ(f32 nearZ);

		void SetOrthographic(bool isOrthographic);
		void SetResolution(const JVector2& resolution);

		bool SetTargetTexture(SharedPtr<ITexture> texture, u8 slot = 0);
		bool SetTargetDepthTexture(SharedPtr<ITexture> texture);


		void SetTargetLayer(const String& layerName);
		void SetCullingLayerMask(u64 mask);
		void SetDepth(i64 depth);
		void SetEnable(bool enable);
	public:
		const JVector3& GetLocation() const;
		JVector3 GetRotation() const;
		const JMatrix& GetViewProjMatrix() const;
		const JMatrix& GetViewMatrix() const;
		const JMatrix& GetProjMatrix() const;

		f32 GetFOV() const;
		f32 GetFarZ() const;
		f32 GetNearZ() const;

		JVector3 GetLook()  const;
		JVector3 GetRight() const;
		JVector3 GetUp()    const;

		bool IsOrthographic() const;
		float GetAspectRatio() const;
		const JVector2& GetResolution() const;
		SharedPtr<ITexture> GetTargetTexture(u8 slot = 0) const;
		const List<SharedPtr<ITexture>>& GetTargetTextures() const;
		SharedPtr<ITexture> GetTargetDepthTexture() const;

		const String& GetTargetLayer() const;
		u64 GetCullingLayerMask() const;
		i64 GetDepth() const;
		bool IsEnable() const;
	private:
		bool UpdateProj() const;
		bool UpdateView() const;
	public:
		static SharedPtr<Camera> Create(const JVector2& resolution, float fov, float nearZ, float farZ, bool isOrthographic);
	};
}

