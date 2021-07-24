#include "pch.h"
#include "StaticMeshRenderer.h"
#include "Transform.h"

#include "Common/DragAndDrop.h"
#include "Graphics/Resource.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Mesh.h"
#include "Class/UI/UIViewModel/ContentsViewModel.h"
#include "Class/Game/GameWorld.h"


namespace JG
{
	StaticMeshRenderer::StaticMeshRenderer()
	{
		mStaticRI = CreateSharedPtr<StandardStaticMeshRenderItem>();
		mStaticRI->Materials.resize(1);
		mStaticRI->Materials[0] = IMaterial::Create("DefaultMaterial", ShaderLibrary::GetInstance().GetShader(ShaderScript::Standard3DShader));
	}
	void StaticMeshRenderer::Awake()
	{
	}
	void StaticMeshRenderer::Start()
	{
	}
	void StaticMeshRenderer::Destory()
	{

	}
	void StaticMeshRenderer::Update()
	{
		if (mMeshID.IsValid() && mMeshAsset == nullptr)
		{
			mMeshAsset = AssetDataBase::GetInstance().GetAsset<IMesh>(mMeshID);
		}
	}
	void StaticMeshRenderer::LateUpdate()
	{
	}
	void StaticMeshRenderer::SetMesh(const String& path)
	{
		mMeshAsset = nullptr;
		mMeshID = AssetDataBase::GetInstance().LoadOriginAsset(path);
		mMeshPath = path;
	}
	void StaticMeshRenderer::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		BaseRenderer::MakeJson(jsonData);
		jsonData->AddMember("MeshPath", mMeshPath);
	}
	void StaticMeshRenderer::LoadJson(SharedPtr<JsonData> jsonData)
	{
		BaseRenderer::LoadJson(jsonData);
		auto val = jsonData->GetMember("MeshPath");
		if (val && val->IsString())
		{
			SetMesh(val->GetString());
		}
	}
	SharedPtr<IRenderItem> StaticMeshRenderer::PushRenderItem()
	{
		auto transform = GetOwner()->GetTransform();
		mStaticRI->WorldMatrix = transform->GetWorldMatrix();

		if (mMeshAsset != nullptr && mMeshAsset->Get() != nullptr)
		{
			mStaticRI->Mesh = mMeshAsset->Get();
			GetOwner()->SetBoundingBox(mStaticRI->Mesh->GetBoundingBox());
		}


		return mStaticRI;
	}
	void StaticMeshRenderer::OnChange(const ChangeData& data)
	{
		BaseRenderer::OnChange(data);
	}
	void StaticMeshRenderer::OnInspectorGUI()
	{
		BaseRenderer::OnInspectorGUI();
		ImGui::AlignTextToFramePadding();
		fs::path p = mMeshPath;
		ImGui::Text("Mesh    "); ImGui::SameLine();
		ImGui::InputText("##Mesh Path", p.filename().string().data(), ImGuiInputTextFlags_ReadOnly);
		if (ImGui::BeginDragDropTarget() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			auto payLoad = ImGui::GetDragDropPayload();
			if (payLoad != nullptr)
			{
				IDragAndDropData* ddd = (IDragAndDropData*)payLoad->Data;
				if (ddd->GetType() == JGTYPE(DDDContentsFile))
				{
					auto dddContentsFile = (DDDContentsFile*)ddd;
					SetMesh(dddContentsFile->FilePath);
				}
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Material"); ImGui::SameLine(); ImGui::InputText("##Mesh Path", "None", ImGuiInputTextFlags_ReadOnly);
	}
}