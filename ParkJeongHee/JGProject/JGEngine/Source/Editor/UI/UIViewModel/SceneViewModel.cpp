#include "SceneViewModel.h"
#include "UI/UIModel/SceneModel.h"


namespace JG
{
	bool SceneViewModel::Initialize()
	{
		if (mSceneMdoel == nullptr)
		{
			mSceneMdoel = RegisterUIModel<SceneModel>();
		}

		return true;
	}

	void SceneViewModel::Destroy()
	{
		UIViewModel::Destroy();

	}

	void SceneViewModel::SetMinSize(const JVector2& size)
	{
		mSceneMdoel->SetMinSize(size);
	}

	const JVector2& SceneViewModel::GetMinSize() const
	{
		return mSceneMdoel->GetMinSize();
	}

	void SceneViewModel::SetSceneTexture(SharedPtr<ITexture> sceneTexture)
	{
		mSceneMdoel->SetSceneTexture(sceneTexture);
	}
	SharedPtr<ITexture> SceneViewModel::GetSceneTexture() const
	{
		return mSceneMdoel->GetSceneTexture();
	}
}

