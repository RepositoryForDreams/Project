#include "pch.h"
#include "SpriteRenderer.h"
#include "Transform.h"

#include "Class/Game/GameWorld.h"
#include "Class/Asset/Asset.h"
namespace JG
{
	void SpriteRenderer::Start()
	{
		BaseRenderer::Start();
		mIsRunning = true;
		ScheduleByFrame(0, 0, -1, 0, [&]() -> EScheduleResult
		{
			if (mIsRunning == true)
			{
				RequestPushRenderItemEvent e;
				e.RenderItem = mSpriteRI;
				SendEvent(e);
				return EScheduleResult::Continue;
			}
			else
			{
				return EScheduleResult::Break;
			}
		});

		GetGameWorld()->GetAssetManager()->AsyncLoadAsset(//"")

		mSpriteRI->TargetLayer == TT(""); // ���̾� -> ���� ���
		mSpriteRI->Color = Color::White(); // �÷�  -> �⺻
		mSpriteRI->WorldMatrix = JMatrix::Identity(); // ���� ��Ʈ���� -> Transform ��
		mSpriteRI->Texture = nullptr; // �ؽ��� -> 
	}
	void SpriteRenderer::Destory()
	{
		BaseRenderer::Destory();
		mIsRunning = false;
	}

	void SpriteRenderer::OnChange(const ChangeData& data)
	{
		BaseRenderer::OnChange(data);

		if (data.Type == JGTYPE(Transform))
		{
			auto transform = static_cast<Transform*>(data.Object);
			// matrix 



		}

	}
	void SpriteRenderer::OnInspectorGUI()
	{
		BaseRenderer::OnInspectorGUI();
	}
}
