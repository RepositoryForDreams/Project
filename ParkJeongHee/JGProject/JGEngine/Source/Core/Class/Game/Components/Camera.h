#pragma once
#include "Class/Game/GameComponent.h"



namespace JG
{
	class Camera : public GameComponent
	{
	private:
		friend class GraphicsSystemLayer;
	private:
		JGCLASS
		mutable bool mIsViewDirty = true;
		mutable bool mIsProjDirty = true;
		//
		mutable JMatrix  mProjMatrix;
		mutable JMatrix  mViewMatrix;
		mutable JMatrix  mViewProjMatrix;
		//
		JVector2 mResolution;
		//
		f32 mNearZ = 0.0f;
		f32 mFarZ = 0.0f;
		f32 mFov = 0.0f;
		//
		i64 mDepth = 0;
		u64 mCullingLayerMask = JG_U64_MAX;
		//
		bool mIsOrthographic = false;
		//
		List<SharedPtr<ITexture>> mTargetTextures;
		SharedPtr<ITexture>       mTargetDepthTexture;
		// RenderingPath
		// Default 
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destory() override;
	protected:
		virtual void Serialize(FileStreamWriter* writer)   const override;
		virtual void DeSerialize(FileStreamReader* reader) override;
	public:
		void SetFOV(f32 fov);
		void SetFarZ(f32 farZ);
		void SetNearZ(f32 nearZ);

		void SetOrthographic(bool isOrthographic);
		void SetResolution(const JVector2& resolution);
		void SetCullingLayerMask(u64 mask);
		void SetDepth(i64 depth);
	public:
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

		u64 GetCullingLayerMask() const;
		i64 GetDepth() const;
	private:
		bool UpdateProj() const;
		bool UpdateView() const;
	public:
		virtual ~Camera() = default;
	protected:
		virtual void OnChange(const ChangeData& data) override;
		virtual void OnInspectorGUI() override;

	};
}