#pragma once
#include "JGCore.h"
#include "Graphics/Camera.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Class/SystemLayer.h"

namespace JG
{
	class GraphicsLayer : public ISystemLayer
	{
		SharedPtr<Camera>    mCamera;
		SharedPtr<IMesh>     mMesh;
		SharedPtr<IMaterial> mMaterial;
		SharedPtr<IShader>   mShader;
		SharedPtr<ITexture>  mTestTexture;
	public:
		virtual ~GraphicsLayer() {}
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;


		virtual void Begin() override;
		virtual void Destroy() override;

		virtual void OnEvent(IEvent& e) override;

		virtual String GetLayerName() override;

		EScheduleResult Update();
	};
}