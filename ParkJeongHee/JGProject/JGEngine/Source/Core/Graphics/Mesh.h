#pragma once
#include "JGCore.h"
#include "InputLayout.h"

// VertexArray
// VertexBuffer ¿Í IndexBuffer ¹­À½
namespace JG
{
	class IVertexBuffer;
	class IIndexBuffer;


	class ISubMesh
	{
	public:
		virtual void SetVertexBuffer(SharedPtr<IVertexBuffer> vertexBuffer) = 0;
		virtual void SetIndexBuffer(SharedPtr<IIndexBuffer> indexBuffer)    = 0;

	public:
		virtual void SetName(const String& name) = 0;
		virtual const String& GetName() const = 0;
		virtual u32 GetIndexCount() const = 0;
		virtual bool Bind() = 0;

	public:
		static SharedPtr<ISubMesh> Create(const String& name);
	};
	class IMesh
	{
	public:
		virtual void AddMesh(SharedPtr<ISubMesh> subMesh) = 0;
		virtual void SetInputLayout(SharedPtr<InputLayout> inputLayout)     = 0;


		virtual SharedPtr<ISubMesh> GetSubMesh(i32 index) = 0;
		virtual u64 GetSubMeshCount() const = 0;

	public:
		virtual void SetName(const String& name) = 0;
		virtual const String& GetName()   = 0;
		virtual bool Bind() = 0;
	public:
		static SharedPtr<IMesh> Create(const String& name);
		static SharedPtr<IMesh> CreateFromFile(const String& path);
	};
}