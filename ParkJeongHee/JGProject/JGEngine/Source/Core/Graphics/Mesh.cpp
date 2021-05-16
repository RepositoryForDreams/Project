#include "Mesh.h"
#include "pch.h"
#include "Mesh.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "Graphics/Resource.h"
#include "Class/Asset/Asset.h"
#include "Class/Asset/AssetImporter.h"


namespace JG
{
    SharedPtr<IMesh> IMesh::Create(const String& name)
    {
        auto api = Application::GetInstance().GetGraphicsAPI();
        JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

        
        return api->CreateMesh(name);
    }
    SharedPtr<IMesh> IMesh::CreateFromFile(const String& path)
    {
        FileStreamReader reader;
        if (reader.Open(path) == true)
        {
            StaticMeshAssetStock meshAsset;
            reader.Read(&meshAsset);

            auto mesh = IMesh::Create(path);
            if (mesh != nullptr)
            {
                u64 vertexCount = meshAsset.Vertices.size();
                for (u64 i = 0; i < vertexCount; ++i)
                {
                    auto& subMeshName = meshAsset.SubMeshNames[i];
                    auto& subVertices = meshAsset.Vertices[i];

                    auto vertexBuffer = IVertexBuffer::Create(subMeshName + TT("_VertexBuffer"), EBufferLoadMethod::CPULoad);
                    vertexBuffer->SetData(subVertices.data(), sizeof(JGVertex), subVertices.size());
                    mesh->AddVertexBuffer(vertexBuffer);
                }
                auto indexBuffer = IIndexBuffer::Create(meshAsset.Name + TT("_IndexBuffer"), EBufferLoadMethod::CPULoad);
                indexBuffer->SetData(meshAsset.Indices.data(), meshAsset.Indices.size());
                mesh->SetIndexBuffer(indexBuffer);
                mesh->SetInputLayout(JGVertex::GetInputLayout());
            }
            reader.Close();

            return mesh;
        }
        return nullptr;
    }
}