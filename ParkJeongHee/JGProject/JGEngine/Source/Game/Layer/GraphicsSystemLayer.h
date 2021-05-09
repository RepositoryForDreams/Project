#pragma once
#include "JGCore.h"
#include "Graphics/Camera.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Class/SystemLayer.h"

namespace JG
{

	
	
	/// <summary>
	/// 1. RenderItem
	/// ���������� �׷��� ���̾ ������������ ������.
	/// 
	/// 2. GameLayerSystem (�ý��� ���̾� ���̹� ����) (���� ���̾� ���̹� ����)
	/// ī�޶󿡼� ���̾� ������ �ø� ����
	/// ���� ���̾�
	/// 3. CameraSystem
	/// Camera�ȿ��� ���� ī�޶� �����Ҽ��ִ�. �׷��� ���̾�� ���� ī�޶� ���������� ���� ī�޶� �ȿ� ī�޶� Texture ���� �׸�
	/// 
	/// 4. RegisterCamera
	/// ī�޶� �����ؼ� �׷��� ���̾ ��� �� ���� ����
	/// </summary>
	class GraphicsSystemLayer : public ISystemLayer
	{
		// ����� ī�޶� ��� �� �������� ���
	private:
		// �̰͵� �ø� ����
		SharedPtr<Camera> mMainCamera = nullptr;


		// ��ī�޶�� �ø� �Ǵ� 
		Dictionary<Camera*, SharedPtr<Camera>> mLayerCameras;
		List<SharedPtr<IRenderItem>> mPushedRenderItemList;
		// ���� ī�޶�
		// ī�޶���� ī�޶�
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
		void Rendering(SharedPtr<IRenderItem> renderItem);
	private:
		EScheduleResult Update();
	private:
		void LoadShaderScript();
	};
}