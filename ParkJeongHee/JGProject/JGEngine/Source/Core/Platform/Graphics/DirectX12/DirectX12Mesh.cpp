#include "pch.h"
#include "DirectX12Mesh.h"


#include "DirectX12API.h"
#include "Utill/PipelineState.h"
#include "Utill/CommandList.h"

namespace JG
{






	void DirectX12Mesh::AddVertexBuffer(SharedPtr<IVertexBuffer> vertexBuffer)
	{
		mVertexBufferList.push_back(vertexBuffer);
	}

	void DirectX12Mesh::SetIndexBuffer(SharedPtr<IIndexBuffer> indexBuffer)
	{
		mIndexBuffer = indexBuffer;
	}

	void DirectX12Mesh::SetInputLayout(SharedPtr<InputLayout> inputLayout)
	{
		mInputLayout = inputLayout;
	}



	void DirectX12Mesh::SetName(const String& name)
	{
		mName = name;
	}

	const String& DirectX12Mesh::GetName()
	{
		return mName;
	}

	u32 DirectX12Mesh::GetIndexCount() const
	{
		if (mIndexBuffer == nullptr || mIndexBuffer->IsValid() == false)
		{
			return 0;
		}
		return mIndexBuffer->GetIndexCount();
	}
	bool DirectX12Mesh::Bind()
	{
		if (mInputLayout == nullptr)
		{
			return false;
		}
		if (mVertexBufferList.empty())
		{
			return false;
		}
		if (mIndexBuffer == nullptr)
		{
			return false;
		}

		// inpuyLayout 설정
		auto pso = DirectX12API::GetGraphicsPipelineState();
		pso->BindInputLayout(*mInputLayout);
		pso->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);


		// vertexBuffer, IndexBuffer 바인딩
		auto commandList = DirectX12API::GetGraphicsCommandList();
		commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		for (auto& vBuffer : mVertexBufferList)
		{
			auto dx12VBuffer = static_cast<DirectX12VertexBuffer*>(vBuffer.get());
			dx12VBuffer->Bind();
		}
		commandList->FlushVertexBuffer();

		auto dx12IBuffer = static_cast<DirectX12IndexBuffer*>(mIndexBuffer.get());
		dx12IBuffer->Bind();

		return true;
	}
}