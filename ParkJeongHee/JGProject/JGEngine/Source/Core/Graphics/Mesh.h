#pragma once
#include "JGCore.h"
#include "InputLayout.h"

// VertexArray
// VertexBuffer ¿Í IndexBuffer ¹­À½
namespace JG
{
	class IVertexBuffer;
	class IIndexBuffer;
	class IMesh
	{
		friend class Renderer2D;
		friend class Renderer;
	public:
		virtual void AddVertexBuffer(SharedPtr<IVertexBuffer> vertexBuffer) = 0;
		virtual void SetIndexBuffer(SharedPtr<IIndexBuffer> indexBuffer) = 0;
		virtual void SetInputLayout(SharedPtr<InputLayout> inputLayout) = 0;



		virtual void SetName(const String& name) = 0;
		virtual const String& GetName() = 0;
		virtual u32 GetIndexCount() const = 0;
	protected:
		virtual bool Bind() = 0;
	public:
		static SharedPtr<IMesh> Create(const String& name);
		static SharedPtr<IMesh> CreateFromFile(const String& path);
	};
}