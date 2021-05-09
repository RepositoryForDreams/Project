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
	/// 렌더러에서 그래픽 레이어에 렌더아이템을 보낸다.
	/// 
	/// 2. GameLayerSystem (시스템 레이어 네이밍 변경) (게임 레이어 네이밍 변경)
	/// 카메라에서 레이어 단위로 컬링 가능
	/// 게임 레이어
	/// 3. CameraSystem
	/// Camera안에서 메인 카메라를 설정할수있다. 그래픽 레이어는 메인 카메라에 최종적으로 메인 카메라 안에 카메라 Texture 들을 그림
	/// 
	/// 4. RegisterCamera
	/// 카메라를 생성해서 그래픽 레이어에 등록 및 해제 가능
	/// </summary>
	class GraphicsSystemLayer : public ISystemLayer
	{
		// 여기는 카메라 등록 및 렌더링만 담당
	private:
		// 이것도 컬링 가능
		SharedPtr<Camera> mMainCamera = nullptr;


		// 이카메라로 컬링 또는 
		Dictionary<Camera*, SharedPtr<Camera>> mLayerCameras;
		List<SharedPtr<IRenderItem>> mPushedRenderItemList;
		// 메인 카메라
		// 카메라안의 카메라
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