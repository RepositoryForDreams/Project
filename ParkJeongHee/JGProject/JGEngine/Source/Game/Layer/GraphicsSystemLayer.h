#pragma once
#include "JGCore.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/GraphicsDefine.h"
#include "Class/Asset/Asset.h"
#include "Class/SystemLayer.h"

namespace JG
{

	
	
	// Global Shader Setting
	// Sampler 추가
	// 오브젝트 인스턴스, 카메라 정보는 무조건 들어있음
	

	class IMesh;
	class IVertexBuffer;
	class IIndexBuffer;
	class IMaterial;
	class GraphicsSystemLayer : public ISystemLayer
	{
		struct CameraItem
		{
			Camera* Camera = nullptr;
			SharedPtr<Renderer2D> Renderer2D;
		};
		class MainCamera
		{
		private:
			JVector2 mResolution;
			JMatrix  mViewProj;
			SharedPtr<ITexture> mTargetTexture;
		public:
			MainCamera() = default;
			MainCamera(const JVector2& resolution) {

				if (mResolution == resolution) return;
				mResolution = resolution;
				auto proj = JMatrix::OrthographicLH(resolution.x, resolution.y, 0.1f, 1000.0f);
				auto view = JMatrix::LookAtLH(JVector3(0, 0, -1.0f), JVector3(0.0f, 0.0f, 1.0f), JVector3(0.0f, 1.0f, 0.0f));
				mViewProj = view * proj;


				TextureInfo info = {};
				info.ArraySize = 1; info.Flags = ETextureFlags::Allow_RenderTarget;
				info.Format = ETextureFormat::R8G8B8A8_Unorm;
				info.Width  = resolution.x;
				info.Height = resolution.y;
				info.MipLevel = 1;
				mTargetTexture = ITexture::Create(TT("MainTexture"), info);
			}
			void Clear()
			{
				mTargetTexture.reset();
			}

			SharedPtr<ITexture> GetTargetTexture() const {
				return mTargetTexture;
			}
			const JMatrix& GetViewProj() const {
				return mViewProj;
			}
			const JVector2& GetResolution() const {
				return mResolution;
			}
		};



		enum class ERenderingState
		{
			Wait,
			ReadyCompelete,
			RenderRegisteredCamera,
			RenderMainCamera,
		};
		enum class ERenderItemPriority : u64
		{
			Default = 0,
			_2D = 100,
		};
	private:
		// 이것도 컬링 가능
		MainCamera  mMainCamera;
		SharedPtr<Renderer2D> mRenderer2D;
		// 이카메라로 컬링 또는 
		Dictionary<Camera*, CameraItem>   mRegisteredCameras;
		SortedDictionary<i64, CameraItem> mSortedLayerCameraList;
		Dictionary<Type, u64> mRenderItemPriority;
		SortedDictionary<u64, Dictionary<Type, List<SharedPtr<IRenderItem>>>> mPushedRenderItems;
		ERenderingState mRenderingState = ERenderingState::Wait;
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
		void Rendering(const CameraItem& cameraItem, Type type, const List<SharedPtr<IRenderItem>>& renderItemList);
		GraphicsSystemLayer::CameraItem CreateLayerCamera(Camera* camera);
	private:
		EScheduleResult Update();
	private:
		void LoadShaderScript();
		
	};
}