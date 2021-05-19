#include "pch.h"
#include "SpriteRenderer.h"
#include "Transform.h"

#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"

#include "Class/Game/GameWorld.h"
#include "Class/Asset/Asset.h"
namespace JG
{
	void SpriteRenderer::Awake()
	{
		mSpriteRI = CreateUniquePtr<Standard2DRenderItem>();
		mSpriteRI->TargetLayer = GetOwner()->GetLayer();
		mSpriteRI->WorldMatrix = JMatrix::Identity();
		mSpriteRI->Color = Color::White();
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

	}

	SharedPtr<IRenderItem> SpriteRenderer::PushRenderItem()
	{
		return mSpriteRI;
	}

	void SpriteRenderer::OnChange(const ChangeData& data)
	{
		BaseRenderer::OnChange(data);

		if (data.Type == JGTYPE(Transform))
		{
			auto transform = static_cast<Transform*>(data.Object);
			mSpriteRI->WorldMatrix = transform->GetWorldMatrix();
		}

	}
	void SpriteRenderer::OnInspectorGUI()
	{
		BaseRenderer::OnInspectorGUI();
		
		ImGui::ColorEdit4("Color", (float*)(&mSpriteRI->Color));

	}
}
