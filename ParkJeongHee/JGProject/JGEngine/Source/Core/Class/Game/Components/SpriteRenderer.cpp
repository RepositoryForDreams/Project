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
	void SpriteRenderer::Serialize(FileStreamWriter* writer)   const
	{
		BaseRenderer::Serialize(writer);
		writer->Write(mSpriteRI->Color);
		writer->Write(mSpriteRI->SortingLayer);
		writer->Write(mSpriteRI->SortingOrder);
		// Texture -> 텍스쳐 경로
	}
	void SpriteRenderer::DeSerialize(FileStreamReader* reader) 
	{
		BaseRenderer::DeSerialize(reader);
		Color  color;
		String sortingLayer;
		i64    sortingOrder = 0;

		reader->Read(&color);
		reader->Read(&sortingLayer);
		reader->Read(&sortingOrder);

		mSpriteRI->Color = color;
		mSpriteRI->SortingLayer = sortingLayer;
		mSpriteRI->SortingOrder = sortingOrder;
	}
	void SpriteRenderer::Update()
	{
	}

	void SpriteRenderer::LateUpdate()
	{

	}

	SharedPtr<IRenderItem> SpriteRenderer::PushRenderItem()
	{
		auto transform = GetOwner()->GetTransform();
		mSpriteRI->WorldMatrix = transform->GetWorldMatrix();
		return mSpriteRI;
	}

	void SpriteRenderer::OnChange(const ChangeData& data)
	{
		BaseRenderer::OnChange(data);

	}
	void SpriteRenderer::OnInspectorGUI()
	{
		BaseRenderer::OnInspectorGUI();
		
		ImGui::ColorEdit4("Color", (float*)(&mSpriteRI->Color));

	}
}
