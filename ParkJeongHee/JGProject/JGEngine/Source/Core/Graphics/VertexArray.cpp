#include "pch.h"
#include "VertexArray.h"


namespace JG
{
    SharedPtr<IVertexArray> IVertexArray::Create()
    {
        return SharedPtr<IVertexArray>();
    }
}