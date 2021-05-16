#pragma once
#include "JGCore.h"
#include "Graphics/Camera.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/GraphicsDefine.h"
#include "Class/Asset/Asset.h"
#include "Class/SystemLayer.h"

namespace JG
{

	
	
	// Global Shader Setting
	// Sampler �߰�
	// ������Ʈ �ν��Ͻ�, ī�޶� ������ ������ �������
	

	class IMesh;
	class IVertexBuffer;
	class IIndexBuffer;
	class IMaterial;
	class GraphicsSystemLayer : public ISystemLayer
	{
		struct LayerCamera
		{
			SharedPtr<Camera> Camera = nullptr;
			SharedPtr<IMesh>  Mesh;
		};
	private:
		// �̰͵� �ø� ����
		SharedPtr<Camera>    mMainCamera = nullptr;
		SharedPtr<IMaterial> mLayerMaterial = nullptr;
		SharedPtr<ITexture>  mNullTexture = nullptr;
		// ��ī�޶�� �ø� �Ǵ� 
		Dictionary<Camera*, LayerCamera> mLayerCameras;
		List<SharedPtr<IRenderItem>> mPushedRenderItemList;

		bool mIsRenderingReady = false;
	public:
		virtual ~GraphicsSystemLayer() {}
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void Begin() override;
		virtual void Destroy() override;

		virtual void OnEvent(IEvent& e) override;

		virtual String GetLayerName() override;
	private:
		bool ResponsePushRenderItem(RequestPushRenderItemEvent& e);
		bool ResponseGetMainSceneTexture(RequestGetMainSceneTextureEvent& e);

		bool ResponseRegisterCamera(RequestRegisterCameraEvent& e);
		bool ResponseUnRegisterCamera(RequestUnRegisterCameraEvent& e);
	private:
		void Rendering(SharedPtr<Camera> camera, SharedPtr<IRenderItem> renderItem);
		GraphicsSystemLayer::LayerCamera CreateLayerCamera(SharedPtr<Camera> camera);
	private:
		EScheduleResult Update();
	private:
		void LoadShaderScript();
		
	};
}