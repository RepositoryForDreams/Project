#pragma once
#include "JGCore.h"
#include "InputLayout.h"

// VertexArray
// VertexBuffer ¿Í IndexBuffer ¹­À½
namespace JG
{
	class IVertexBuffer;
	class IIndexBuffer;
	class IVertexArray
	{
		
	public:
		virtual void AddVertexBuffer(SharedPtr<IVertexBuffer> vertexBuffer) = 0;
		virtual void SetIndexBuffer(SharedPtr<IIndexBuffer> indexBuffer) = 0;
		virtual void SetInputLayout(SharedPtr<InputLayout> inputLayout) = 0;
		virtual void Bind() = 0;
	public:
		static SharedPtr<IVertexArray> Create();
	};
}