#include "pch.h"
#include "SceneViewModel.h"
#include "Class/UI/UIModel/SceneModel.h"


namespace JG
{
	void SceneViewModel::Initialize()
	{
		mSceneMdoel = RegisterUIModel<SceneModel>();
		SharedPtr<ITexture> texture;
		RequestGetMainSceneTextureEvent e;
		SendEventImmediate(e);
		mSceneMdoel->SetSceneTexture(e.SceneTexture);
	}

	void SceneViewModel::Destroy()
	{
		UIViewModel::Destroy();

	}

	void SceneViewModel::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<NotifyChangeMainSceneTextureEvent>(EVENT_BIND_FN(&SceneViewModel::NotifyChangeMainSceneTexture));

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
	bool SceneViewModel::NotifyChangeMainSceneTexture(NotifyChangeMainSceneTextureEvent& e)
	{
		if (mSceneMdoel != nullptr)
		{
			mSceneMdoel->SetSceneTexture(e.SceneTexture);
		}
		return false;
	}
}

