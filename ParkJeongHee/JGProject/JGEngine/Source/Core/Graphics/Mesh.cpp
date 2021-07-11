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
    SharedPtr<ISubMesh> ISubMesh::Create(const String& name)
    {
        auto api = Application::GetInstance().GetGraphicsAPI();
        JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");


        return api->CreateSubMesh(name);
    }
    SharedPtr<IMesh> IMesh::Create(const String& name)
    {
        auto api = Application::GetInstance().GetGraphicsAPI();
        JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

        
        return api->CreateMesh(name);
    }
    SharedPtr<IMesh> IMesh::CreateFromFile(const String& path)
    {

        return nullptr;
    }

}