#pragma once
#include "JGCore.h"
#include "Graphics/GraphicsDefine.h"
#include "Graphics/Resource.h"
#include "Class/UI/UIViewModel.h"


namespace JG
{
	class SceneViewModel : public UIViewModel
	{
		class SceneModel* mSceneMdoel = nullptr;
	public:
		virtual ~SceneViewModel() = default;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;

		virtual void OnEvent(IEvent& e);
	public:
		void SetMinSize(const JVector2& size);
		const JVector2& GetMinSize() const;


		void SetSceneTexture(SharedPtr<ITexture> sceneTexture);
		SharedPtr<ITexture> GetSceneTexture() const;
	private:
		bool NotifyChangeMainSceneTexture(NotifyChangeMainSceneTextureEvent& e);
	};
}