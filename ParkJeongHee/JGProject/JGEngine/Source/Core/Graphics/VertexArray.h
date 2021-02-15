#pragma once
#include "JGCore.h"
#include "InputLayout.h"

// VertexArray
// VertexBuffer ¿Í IndexBuffer ¹­À½
namespace JG
{
	class IVertexArray
	{
	public:
		virtual void AddVertex(void* vData, u64 elementSize, u64 elementCount, u32* iData, u64 indexCount) = 0;
		virtual void SetInputLayout(SharedPtr<InputLayout> inputLayout) = 0;
		virtual void Bind() = 0;
	public:
		static SharedPtr<IVertexArray> Create();
	};
}