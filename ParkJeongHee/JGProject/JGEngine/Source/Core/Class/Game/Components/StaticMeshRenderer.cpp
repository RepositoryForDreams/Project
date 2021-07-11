#include "pch.h"
#include "StaticMeshRenderer.h"
#include "Common/DragAndDrop.h"
#include "Class/UI/UIViewModel/ContentsViewModel.h"



namespace JG
{
	StaticMeshRenderer::StaticMeshRenderer()
	{
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
	}
	void StaticMeshRenderer::LateUpdate()
	{
	}
	void StaticMeshRenderer::SetMesh(const String& path)
	{
		mMeshAsset = nullptr;
		mMeshID = AssetID();
		mMeshPath = path;
	}
	void StaticMeshRenderer::MakeJson(SharedPtr<JsonData> jsonData) const
	{
	}
	void StaticMeshRenderer::LoadJson(SharedPtr<JsonData> jsonData)
	{
	}
	SharedPtr<IRenderItem> StaticMeshRenderer::PushRenderItem()
	{
		return SharedPtr<IRenderItem>();
	}
	void StaticMeshRenderer::OnChange(const ChangeData& data)
	{
	}
	void StaticMeshRenderer::OnInspectorGUI()
	{
		fs::path p = mMeshPath;
		ImGui::InputText("Mesh", p.filename().string().data(), ImGuiInputTextFlags_ReadOnly);
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
					SetMesh(fileInfo->Path);
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
}