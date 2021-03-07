#pragma once
#include "JGCore.h"
#include "InputLayout.h"

// VertexArray
// VertexBuffer �� IndexBuffer ����
namespace JG
{
	class IVertexBuffer;
	class IIndexBuffer;
	class IMesh
	{
	public:
		virtual void AddVertexBuffer(SharedPtr<IVertexBuffer> vertexBuffer) = 0;
		virtual void SetIndexBuffer(SharedPtr<IIndexBuffer> indexBuffer) = 0;
		virtual void SetInputLayout(SharedPtr<InputLayout> inputLayout) = 0;
		virtual bool Bind() = 0;


		virtual void SetName(const String& name) = 0;
		virtual const String& GetName() = 0;
	public:
		static SharedPtr<IMesh> Create(const String& name);
	};
}