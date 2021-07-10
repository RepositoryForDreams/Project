#include "pch.h"
#include "SpriteRenderer.h"
#include "Transform.h"

#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
#include "Class/Game/GameWorld.h"
#include "Class/UI/UIViewModel/ContentsViewModel.h"
#include "Common/DragAndDrop.h"
namespace JG
{
	SpriteRenderer::SpriteRenderer()
	{
		mSpriteRI = CreateUniquePtr<Standard2DRenderItem>();
		mSpriteRI->WorldMatrix = JMatrix::Identity();
		mSpriteRI->Color = Color::White();
	}
	void SpriteRenderer::Awake()
	{
		mSpriteRI->TargetLayer = GetOwner()->GetLayer();
	}
	void SpriteRenderer::Start()
	{
		BaseRenderer::Start();
		auto assetPath = Application::GetAssetPath();
		mSpritePath = CombinePath(assetPath, TT("Resources/NullTexture.jgasset"));
	}
	void SpriteRenderer::Destory()
	{
		BaseRenderer::Destory();
	}
	void SpriteRenderer::Update()
	{
		if (mSpriteID.IsValid() == false)
		{
			mSpriteID = GetGameWorld()->GetAssetManager()->LoadAsset(mSpritePath);
			mAsset = GetGameWorld()->GetAssetManager()->GetAsset<ITexture>(mSpriteID);
		}
	}

	void SpriteRenderer::LateUpdate()
	{

	}

	void SpriteRenderer::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		BaseRenderer::MakeJson(jsonData);
		jsonData->AddMember("Color", JVector4(mSpriteRI->Color));
	}

	void SpriteRenderer::LoadJson(SharedPtr<JsonData> jsonData)
	{
		BaseRenderer::LoadJson(jsonData);
		auto val = jsonData->GetMember("Color");
		if (val)
		{
			mSpriteRI->Color = Color(val->GetVector4());
		}
	}

	void SpriteRenderer::SetSprite(const String& path)
	{
		mAsset = nullptr;
		mSpriteID = AssetID();
		mSpritePath = path;
	}

	SharedPtr<IRenderItem> SpriteRenderer::PushRenderItem()
	{
		auto transform = GetOwner()->GetTransform();
		mSpriteRI->WorldMatrix = transform->GetWorldMatrix();

		if (mAsset != nullptr && mAsset->Get())
		{
			auto info = mAsset->Get()->GetTextureInfo();
			mSpriteRI->Texture = mAsset->Get();
			mSpriteRI->WorldMatrix = JMatrix::Scaling(JVector3(info.Width, info.Height, 1)) * mSpriteRI->WorldMatrix;
		}
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
		
		auto path = ws2s(mSpritePath);
		ImGui::InputText("Texture", path.data(), ImGuiInputTextFlags_ReadOnly);




		
		if (ImGui::BeginDragDropTarget() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			auto payLoad = ImGui::GetDragDropPayload();
			if (payLoad != nullptr)
			{
				IDragAndDropData* ddd = (IDragAndDropData*)payLoad->Data;
				if (ddd->GetType() == JGTYPE(DDDContentsFile))
				{
					auto dddContentsFile = (DDDContentsFile*)ddd;
					auto fileInfo = (ContentsFileInfo*)(dddContentsFile->pFileInfoPtr);
					SetSprite(fileInfo->Path);
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
}
