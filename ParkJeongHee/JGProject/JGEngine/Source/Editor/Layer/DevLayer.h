#pragma once
#include "JGCore.h"
#include "Class/Layer.h"


namespace JG
{
	class Camera;
	class IFrameBuffer;
	class ITexture;
	class IShader;
	class IComputer;
	class IComputeBuffer;
	class DevLayer : public ILayer
	{
	private:
		//SharedPtr<IFrameBuffer> mFrameBuffer;
		SharedPtr<Camera>     mCamera;
	public:
		virtual ~DevLayer() = default;
	public:
		virtual void OnAttach() override;
		virtual void OnDetach()  override;
		virtual void Update() override;
		virtual void LateUpdate() override;
		virtual void Begin() override;
		virtual void Destroy()  override;
		virtual void OnEvent(IEvent& e)  override;
		virtual String GetLayerName()  override;
	private:
		bool Resize(AppResizeEvent& e);
	};
}