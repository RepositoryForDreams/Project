#include "pch.h"
#include "Mesh.h"
#include "Application.h"
#include "GraphicsAPI.h"

namespace JG
{
    SharedPtr<IMesh> IMesh::Create(const String& name)
    {
        auto api = Application::GetInstance().GetGraphicsAPI();
        JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

        
        return api->CreateMesh(name);
    }
}