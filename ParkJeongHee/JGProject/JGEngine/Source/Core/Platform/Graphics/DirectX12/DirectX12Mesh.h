#pragma once
#include "Graphics/Mesh.h"
#include "Graphics/InputLayout.h"
#include "Graphics/Resource.h"

namespace JG
{
	class DirectX12Mesh : public IMesh
	{

		SharedPtr<InputLayout> mInputLayout;
		List<SharedPtr<IVertexBuffer>> mVertexBufferList;
		SharedPtr<IIndexBuffer> mIndexBuffer;

		String mName;
	public:
		virtual void AddVertexBuffer(SharedPtr<IVertexBuffer> vertexBuffer) override;
		virtual void SetIndexBuffer(SharedPtr<IIndexBuffer> indexBuffer) override;
		virtual void SetInputLayout(SharedPtr<InputLayout> inputLayout) override;
		virtual bool Bind() override;


		virtual void SetName(const String& name) override;
		virtual const String& GetName() override;
	};
}