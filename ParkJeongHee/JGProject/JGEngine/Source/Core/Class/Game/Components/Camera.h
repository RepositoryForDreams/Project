#pragma once
#include "Class/Game/GameComponent.h"



namespace JG
{
	class Camera : public GameComponent
	{
		static Camera* smMainCamera;
	public:
		static Camera* GetMainCamera();
		static void SetMainCamera(Camera* mainCamera);
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
		Color    mClearColor;
		//
		f32 mNearZ = 0.0f;
		f32 mFarZ  = 0.0f;
		f32 mFov   = 0.0f;
		//
		u64 mCullingLayerMask = JG_U64_MAX;
		//
		bool mIsOrthographic  = false;
		bool mIsMainCamera    = false;

		ERendererPath mRendererPath = ERendererPath::Foward;
	public:
		Camera();
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Update() override;
		virtual void Destory() override;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		void SetFOV(f32 fov);
		void SetFarZ(f32 farZ);
		void SetNearZ(f32 nearZ);

		void SetOrthographic(bool isOrthographic);
		void SetResolution(const JVector2& resolution);
		void SetCullingLayerMask(u64 mask);
		void SetClearColor(const Color& color);
		void SetRendererPath(ERendererPath rendererPath);
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
		const Color& GetClearColor() const;

		u64 GetCullingLayerMask() const;
		ERendererPath GetRendererPath() const;
	private:
		bool UpdateProj() const;
		bool UpdateView() const;
	public:
		virtual ~Camera() = default;
	protected:
		virtual void OnChange(const ChangeData& data) override;
		virtual void OnInspectorGUI() override;

	};

	class EditorCamera : public Camera
	{
	private:
		JGCLASS
	};
}