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
		auto assetPath = Application::GetAssetPath();
		mSpritePath = CombinePath(assetPath, "Resources/NullTexture.jgasset");
	}
	void SpriteRenderer::Awake()
	{
		mSpriteRI->TargetLayer = GetOwner()->GetLayer();
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
		jsonData->AddMember("SpritePath", mSpritePath);

	}

	void SpriteRenderer::LoadJson(SharedPtr<JsonData> jsonData)
	{
		BaseRenderer::LoadJson(jsonData);
		auto val = jsonData->GetMember("Color");
		if (val)
		{
			mSpriteRI->Color = Color(val->GetVector4());
		}
		val = jsonData->GetMember("SpritePath");
		if (val && val->IsString())
		{
			SetSprite(val->GetString());
		}
	}

	void SpriteRenderer::SetSprite(const String& path)
	{
		if (Json::GetAssetFormat(path) != EAssetFormat::Texture)
		{
			return;
		}
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
			f32 adjust = (f32)info.PixelPerUnit / (f32)GameSettings::GetUnitSize();
			f32 spriteWidth  = info.Width  * adjust;
			f32 spriteHeight = info.Height * adjust;

			if (mSpriteSize.x != spriteWidth || mSpriteSize.y != spriteHeight)
			{
				JBBox bbox;
				bbox.min = JVector3(-spriteWidth * 0.5f, -spriteHeight * 0.5f, 0.0f);
				bbox.max = JVector3(spriteWidth * 0.5f, spriteHeight * 0.5f, 0.0f);
				GetOwner()->SetBoundingBox(bbox);
				mSpriteSize = JVector2(spriteWidth, spriteHeight);
				
			}
			mSpriteRI->WorldMatrix = JMatrix::Scaling(JVector3(spriteWidth, spriteHeight, 1)) * mSpriteRI->WorldMatrix;
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
		
		fs::path p = mSpritePath;
		auto fileName = p.filename().string();
		ImGui::InputText("Texture", fileName.data(), ImGuiInputTextFlags_ReadOnly);
		if (ImGui::BeginDragDropTarget() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			auto payLoad = ImGui::GetDragDropPayload();
			if (payLoad != nullptr)
			{
				IDragAndDropData* ddd = (IDragAndDropData*)payLoad->Data;
				if (ddd->GetType() == JGTYPE(DDDContentsFile))
				{
					auto dddContentsFile = (DDDContentsFile*)ddd;
					SetSprite(dddContentsFile->FilePath);
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
}
