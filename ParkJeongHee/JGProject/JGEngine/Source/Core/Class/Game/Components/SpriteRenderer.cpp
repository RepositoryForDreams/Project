#include "pch.h"
#include "SpriteRenderer.h"
#include "Transform.h"

#include "Class/Game/GameWorld.h"
#include "Class/Asset/Asset.h"
namespace JG
{
	void SpriteRenderer::Awake()
	{
		mSpriteRI = CreateUniquePtr<StandardSpriteRenderItem>();
		mSpriteRI->TargetLayer = GetOwner()->GetLayer();
		mSpriteRI->Color = Color::White();
		mSpriteRI->WorldMatrix = JMatrix::Identity();
		mSpriteRI->Texture = nullptr;
	}
	void SpriteRenderer::Start()
	{
		BaseRenderer::Start();

	}
	void SpriteRenderer::Destory()
	{
		BaseRenderer::Destory();
	}

	void SpriteRenderer::Update()
	{
	}

	void SpriteRenderer::LateUpdate()
	{
		RequestPushRenderItemEvent e;
		mSpriteRI->TargetLayer = GetOwner()->GetLayer();
		mSpriteRI->WorldMatrix = JMatrix::Scaling(JVector3(100, 100, 1.0f));
		e.RenderItem = mSpriteRI;

		SendEvent(e);
	}

	void SpriteRenderer::OnChange(const ChangeData& data)
	{
		BaseRenderer::OnChange(data);

		if (data.Type == JGTYPE(Transform))
		{
			auto transform = static_cast<Transform*>(data.Object);
		}

	}
	void SpriteRenderer::OnInspectorGUI()
	{
		BaseRenderer::OnInspectorGUI();
		
	
		ImGui::ColorEdit4("Color", (float*)&mSpriteRI->Color);


	}
}
