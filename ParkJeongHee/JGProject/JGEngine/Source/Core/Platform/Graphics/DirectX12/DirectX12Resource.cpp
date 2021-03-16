#include "pch.h"
#include "DirectX12Resource.h"
#include "DirectX12API.h"
#include "DirectX12Shader.h"
#include "Utill/ResourceStateTracker.h"
#include "Utill/CommandQueue.h"
#include "Utill/DescriptorAllocator.h"
#include "Utill/CommandList.h"
#include "Utill/PipelineState.h"
#include "Utill/ShaderDataForm.h"
namespace JG
{
	DirectX12VertexBuffer::~DirectX12VertexBuffer()
	{
		Reset();
	}


	bool DirectX12VertexBuffer::SetData(void* datas, u64 elementSize, u64 elementCount)
	{
		u64 originBtSize = mElementSize * mElementCount;
		mElementSize = elementSize; mElementCount = elementCount;
		u64 btSize = mElementSize * mElementCount;

		// Create
		switch (mLoadMethod)
		{
		case EBufferLoadMethod::GPULoad:
		{
			Reset();
			auto d3dDevice = DirectX12API::GetD3DDevice();
			HRESULT hResult = d3dDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(btSize),
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				IID_PPV_ARGS(mD3DResource.GetAddressOf()));
			if (SUCCEEDED(hResult))
			{
				ResourceStateTracker::RegisterResource(GetName(), mD3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);

				auto commandList = DirectX12API::GetCopyCommandList();
				commandList->CopyBuffer(mD3DResource.Get(), datas, elementSize, elementCount);
			}
		}
		break;
		case EBufferLoadMethod::CPULoad:
			if (mD3DResource && originBtSize != btSize)
			{
				Reset();
			}
			if (mD3DResource == nullptr)
			{
				auto d3dDevice = DirectX12API::GetD3DDevice();
				HRESULT hResult = d3dDevice->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(btSize),
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(mD3DResource.GetAddressOf()));
				if (SUCCEEDED(hResult))
				{
					ResourceStateTracker::RegisterResource(GetName(), mD3DResource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ);
					mD3DResource->Map(0, nullptr, &mCPUData);
				}
			}
			memcpy(mCPUData, datas, btSize);
			break;
		}

	

		return true;
	}

	bool DirectX12VertexBuffer::IsValid() const
	{
		return mCPUData != nullptr;
	}
	EBufferLoadMethod DirectX12VertexBuffer::GetBufferLoadMethod() const
	{
		return mLoadMethod;
	}
	void DirectX12VertexBuffer::SetBufferLoadMethod(EBufferLoadMethod method)
	{
		 mLoadMethod = method;
	}
	void DirectX12VertexBuffer::Bind()
	{
		if (IsValid() == false)
		{
			return;
		}
		auto commandList = DirectX12API::GetGraphicsCommandList();


		D3D12_VERTEX_BUFFER_VIEW View = {};
		View.BufferLocation = mD3DResource->GetGPUVirtualAddress();
		View.SizeInBytes    = mElementSize * mElementCount;
		View.StrideInBytes  = mElementSize;
		commandList->BindVertexBuffer(View, false);
	}

	void DirectX12VertexBuffer::Reset()
	{
		switch (mLoadMethod)
		{
		case EBufferLoadMethod::CPULoad:
			if (mD3DResource)
			{
				mD3DResource->Unmap(0, nullptr);
				ResourceStateTracker::UnRegisterResource(mD3DResource.Get());
				mD3DResource.Reset(); mD3DResource = nullptr;
				mCPUData = nullptr;
			}
			break;
		case EBufferLoadMethod::GPULoad:
			if (mD3DResource)
			{
				ResourceStateTracker::UnRegisterResource(mD3DResource.Get());
				mD3DResource.Reset(); mD3DResource = nullptr;
			}
			break;
		}
	}




	DirectX12IndexBuffer::~DirectX12IndexBuffer()
	{
		Reset();
	}

	bool DirectX12IndexBuffer::SetData(u32* datas, u64 count)
	{
		u64 originBtSize = sizeof(u32) * mIndexCount;
		mIndexCount = count;
		u64 btSize = sizeof(u32) * mIndexCount;

		// Create
		switch (mLoadMethod)
		{
		case EBufferLoadMethod::GPULoad:
		{
			Reset();
			auto d3dDevice = DirectX12API::GetD3DDevice();
			HRESULT hResult = d3dDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(btSize),
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				IID_PPV_ARGS(mD3DResource.GetAddressOf()));
			if (SUCCEEDED(hResult))
			{
				ResourceStateTracker::RegisterResource(GetName(), mD3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);

				auto commandList = DirectX12API::GetCopyCommandList();
				commandList->CopyBuffer(mD3DResource.Get(), datas, sizeof(u32), mIndexCount);
			}
		}
		break;
		case EBufferLoadMethod::CPULoad:
			if (mD3DResource && originBtSize != btSize)
			{
				Reset();
			}
			if (mD3DResource == nullptr)
			{
				auto d3dDevice = DirectX12API::GetD3DDevice();
				HRESULT hResult = d3dDevice->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(btSize),
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(mD3DResource.GetAddressOf()));
				if (SUCCEEDED(hResult))
				{
					ResourceStateTracker::RegisterResource(GetName(), mD3DResource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ);
					mD3DResource->Map(0, nullptr, (void**)&mCPUData);
				}
			}
			memcpy(mCPUData, datas, btSize);
			break;
		}
		return true;
	}

	bool DirectX12IndexBuffer::IsValid() const
	{
		return mCPUData != nullptr;
	}
	EBufferLoadMethod DirectX12IndexBuffer::GetBufferLoadMethod() const
	{
		return mLoadMethod;
	}
	void DirectX12IndexBuffer::SetBufferLoadMethod(EBufferLoadMethod method)
	{
		mLoadMethod = method;
	}

	void DirectX12IndexBuffer::Bind()
	{
		if (IsValid() == false)
		{
			return;
		}
		auto commandList = DirectX12API::GetGraphicsCommandList();
		D3D12_INDEX_BUFFER_VIEW View;
		View.BufferLocation = mD3DResource->GetGPUVirtualAddress();
		View.Format = DXGI_FORMAT_R32_UINT;
		View.SizeInBytes = sizeof(u32) * mIndexCount;

		commandList->BindIndexBuffer(View);
	}

	void DirectX12IndexBuffer::Reset()
	{
		switch (mLoadMethod)
		{
		case EBufferLoadMethod::CPULoad:
			if (mD3DResource)
			{
				mD3DResource->Unmap(0, nullptr);
				ResourceStateTracker::UnRegisterResource(mD3DResource.Get());
				mD3DResource.Reset(); mD3DResource = nullptr;
				mCPUData = nullptr;
			}
			break;
		case EBufferLoadMethod::GPULoad:
			if (mD3DResource)
			{
				ResourceStateTracker::UnRegisterResource(mD3DResource.Get());
				mD3DResource.Reset(); mD3DResource = nullptr;
			}
			break;
		}
	}

	bool DirectX12ComputeBuffer::SetFloat(const String& name, float value)
	{
		return mShaderData->SetFloat(name, value);
	}
	bool DirectX12ComputeBuffer::SetFloat2(const String& name, const JVector2& value)
	{
		return  mShaderData->SetFloat2(name, value);
	}
	bool DirectX12ComputeBuffer::SetFloat3(const String& name, const JVector3& value)
	{
		return  mShaderData->SetFloat3(name, value);
	}
	bool DirectX12ComputeBuffer::SetFloat4(const String& name, const JVector4& value)
	{
		return  mShaderData->SetFloat4(name, value);
	}
	bool DirectX12ComputeBuffer::SetInt(const String& name, i32 value)
	{
		return mShaderData->SetInt(name, value);
	}
	bool DirectX12ComputeBuffer::SetInt2(const String& name, const JVector2Int& value)
	{
		return mShaderData->SetInt2(name, value);
	}
	bool DirectX12ComputeBuffer::SetInt3(const String& name, const JVector3Int& value)
	{
		return mShaderData->SetInt3(name, value);
	}
	bool DirectX12ComputeBuffer::SetInt4(const String& name, const JVector4Int& value)
	{
		return mShaderData->SetInt4(name, value);
	}
	bool DirectX12ComputeBuffer::SetUint(const String& name, u32 value)
	{
		return mShaderData->SetUint(name, value);
	}
	bool DirectX12ComputeBuffer::SetUint2(const String& name, const JVector2Uint& value)
	{
		return mShaderData->SetUint2(name, value);
	}
	bool DirectX12ComputeBuffer::SetUint3(const String& name, const JVector3Uint& value)
	{
		return mShaderData->SetUint3(name, value);
	}
	bool DirectX12ComputeBuffer::SetUint4(const String& name, const JVector4Uint& value)
	{
		return mShaderData->SetUint4(name, value);
	}
	bool DirectX12ComputeBuffer::SetFloat4x4(const String& name, const JMatrix& value)
	{
		return mShaderData->SetFloat4x4(name, value);
	}

	bool DirectX12ComputeBuffer::SetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture> texture)
	{
		return mShaderData->SetTexture(name, textureSlot, texture);
	}

	bool DirectX12ComputeBuffer::SetFloatArray(const String& name, const List<float>& value)
	{
		return mShaderData->SetFloatArray(name, value);
	}

	bool DirectX12ComputeBuffer::SetFloat2Array(const String& name, const List<JVector2>& value)
	{
		return mShaderData->SetFloat2Array(name, value);
	}

	bool DirectX12ComputeBuffer::SetFloat3Array(const String& name, const List<JVector3>& value)
	{
		return mShaderData->SetFloat3Array(name, value);
	}

	bool DirectX12ComputeBuffer::SetFloat4Array(const String& name, const List<JVector4>& value)
	{
		return mShaderData->SetFloat4Array(name, value);
	}

	bool DirectX12ComputeBuffer::SetIntArray(const String& name, const List<i32>& value)
	{
		return mShaderData->SetIntArray(name, value);
	}

	bool DirectX12ComputeBuffer::SetInt2Array(const String& name, const List<JVector2Int>& value)
	{
		return mShaderData->SetInt2Array(name, value);
	}

	bool DirectX12ComputeBuffer::SetInt3Array(const String& name, const List<JVector3Int>& value)
	{
		return mShaderData->SetInt3Array(name, value);
	}

	bool DirectX12ComputeBuffer::SetInt4Array(const String& name, const List<JVector4Int>& value)
	{
		return mShaderData->SetInt4Array(name, value);
	}

	bool DirectX12ComputeBuffer::SetUintArray(const String& name, const List<u32>& value)
	{
		return mShaderData->SetUintArray(name, value);
	}

	bool DirectX12ComputeBuffer::SetUint2Array(const String& name, const List<JVector2Uint>& value)
	{
		return mShaderData->SetUint2Array(name, value);
	}

	bool DirectX12ComputeBuffer::SetUint3Array(const String& name, const List<JVector3Uint>& value)
	{
		return mShaderData->SetUint3Array(name, value);
	}

	bool DirectX12ComputeBuffer::SetUint4Array(const String& name, const List<JVector4Uint>& value)
	{
		return mShaderData->SetUint4Array(name, value);
	}

	bool DirectX12ComputeBuffer::SetFloat4x4Array(const String& name, const List<JMatrix>& value)
	{
		return mShaderData->SetFloat4x4Array(name, value);
	}

	bool DirectX12ComputeBuffer::SetStructDataArray(const String& name, void* datas, u64 elementCount, u64 elementSize)
	{
		JGASSERT("NOT Supported SetStructDataArray");
		return false;
	}



	bool DirectX12ComputeBuffer::GetFloat(const String& name, float* out_value)
	{
		return mShaderData->GetFloat(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetFloat2(const String& name, JVector2* out_value)
	{
		return mShaderData->GetFloat2(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetFloat3(const String& name, JVector3* out_value)
	{
		return mShaderData->GetFloat3(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetFloat4(const String& name, JVector4* out_value)
	{
		return mShaderData->GetFloat4(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetInt(const String& name, i32* out_value)
	{
		return mShaderData->GetInt(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetInt2(const String& name, JVector2Int* out_value)
	{
		return mShaderData->GetInt2(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetInt3(const String& name, JVector3Int* out_value)
	{
		return mShaderData->GetInt3(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetInt4(const String& name, JVector4Int* out_value)
	{
		return mShaderData->GetInt4(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetUint(const String& name, u32* out_value)
	{
		return mShaderData->GetUint(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetUint2(const String& name, JVector2Uint* out_value)
	{
		return mShaderData->GetUint2(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetUint3(const String& name, JVector3Uint* out_value)
	{
		return mShaderData->GetUint3(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetUint4(const String& name, JVector4Uint* out_value)
	{
		return mShaderData->GetUint4(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetFloat4x4(const String& name, JMatrix* out_value)
	{
		return mShaderData->GetFloat4x4(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture>* out_value)
	{
		return mShaderData->GetTexture(name, textureSlot, out_value);
	}

	bool DirectX12ComputeBuffer::GetFloatArray(const String& name, List<float>* out_value)
	{
		return mShaderData->GetFloatArray(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetFloat2Array(const String& name, List<JVector2>* out_value)
	{
		return mShaderData->GetFloat2Array(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetFloat3Array(const String& name, List<JVector3>* out_value)
	{
		return mShaderData->GetFloat3Array(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetFloat4Array(const String& name, List<JVector4>* out_value)
	{
		return mShaderData->GetFloat4Array(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetIntArray(const String& name, List<i32>* out_value)
	{
		return mShaderData->GetIntArray(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetInt2Array(const String& name, List<JVector2Int>* out_value)
	{
		return mShaderData->GetInt2Array(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetInt3Array(const String& name, List<JVector3Int>* out_value)
	{
		return mShaderData->GetInt3Array(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetInt4Array(const String& name, List<JVector4Int>* out_value)
	{
		return mShaderData->GetInt4Array(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetUintArray(const String& name, List<u32>* out_value)
	{
		return mShaderData->GetUintArray(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetUint2Array(const String& name, List<JVector2Uint>* out_value)
	{
		return mShaderData->GetUint2Array(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetUint3Array(const String& name, List<JVector3Uint>* out_value)
	{
		return mShaderData->GetUint3Array(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetUint4Array(const String& name, List<JVector4Uint>* out_value)
	{
		return mShaderData->GetUint4Array(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetFloat4x4Array(const String& name, List<JMatrix>* out_value)
	{
		return mShaderData->GetFloat4x4Array(name, out_value);
	}

	bool DirectX12ComputeBuffer::GetData(const String& name, void* data)
	{
		return false;
	}

	bool DirectX12ComputeBuffer::IsValid() const
	{
		return false;
	}

	bool DirectX12ComputeBuffer::IsCompelete() const
	{
		return false;
	}

	bool DirectX12ComputeBuffer::Dispatch(u32 groupX, u32 groupY, u32 groupZ)
	{
		if (Bind() == false)
		{
			return false;
		}

		auto commandList = DirectX12API::GetComputeCommandList();
		commandList->Dispatch(groupX, groupY, groupZ);

		return true;
	}

	bool DirectX12ComputeBuffer::Bind() const
	{
	
		auto loadedShader = mShaderData->OwnerShader.lock();
		auto dx12Shader   = static_cast<DirectX12Shader*>(loadedShader.get());

		if (dx12Shader != nullptr)
		{
			auto commandList = DirectX12API::GetComputeCommandList();
			if (dx12Shader->Bind() == false)
			{
				JG_CORE_ERROR("Failed Bind {0} Shader, in {1} ComputeBuffer", ws2s(dx12Shader->GetName()), ws2s(GetName()));
				return false;
			}

			// 상수 버퍼
			auto shaderDataForm = dx12Shader->GetShaderDataForm();
			for (auto& _pair : shaderDataForm->CBufferDataMap)
			{
				auto cBufferName = _pair.first;
				auto cBufferData = _pair.second.get();
				auto& byteData = mShaderData->ByteDatas[cBufferName];
				commandList->BindDynamicConstantBuffer(cBufferData->RootParm, byteData.data(), byteData.size());
			}
			// Texture
			for (auto& _pair : shaderDataForm->TextureDataMap)
			{
				auto textureData = _pair.second.get();
				auto& textureList = mShaderData->TextureDatas[_pair.first];
				u64 textureCount = textureList.size();
				List<D3D12_CPU_DESCRIPTOR_HANDLE> handles;
				for (u64 i = 0; i < textureCount; ++i)
				{
					if (textureList[i] != nullptr && textureList[i]->IsValid())
					{
						handles.push_back(static_cast<DirectX12Texture*>(textureList[i].get())->GetSRV());
					}
				}

				if (handles.empty() == false)
				{
					commandList->BindTextures(textureData->RootParm, handles);
				}
			}
			// RWTexture
			for (auto& _pair : shaderDataForm->RWTextureDataMap)
			{
				auto textureData = _pair.second.get();
				auto& textureList = mShaderData->TextureDatas[_pair.first];
				u64 textureCount = textureList.size();
				List<D3D12_CPU_DESCRIPTOR_HANDLE> handles;
				for (u64 i = 0; i < textureCount; ++i)
				{
					if (textureList[i] != nullptr && textureList[i]->IsValid())
					{
						handles.push_back(static_cast<DirectX12Texture*>(textureList[i].get())->GetUAV());
					}
				}

				if (handles.empty() == false)
				{
					commandList->BindTextures(textureData->RootParm, handles);
				}
			}
			// structuredBuffer
			for (auto& _pair : shaderDataForm->StructuredBufferDataMap)
			{
				auto structuredBufferName = _pair.first;
				auto structuredBufferData = _pair.second.get();
				auto& byteData = mShaderData->ByteDatas[structuredBufferName];
				commandList->BindDynamicStructuredBuffer(
					structuredBufferData->RootParm, byteData.data(), structuredBufferData->ElementDataSize, byteData.size());
			}

			// RWStructuredBuffer
			for (auto& _pair : shaderDataForm->RWStructuredBufferDataMap)
			{
				auto structuredBufferName = _pair.first;
				auto structuredBufferData = _pair.second.get();
				auto& byteData = mShaderData->ByteDatas[structuredBufferName];
				commandList->BindDynamicStructuredBuffer(
					structuredBufferData->RootParm, byteData.data(), structuredBufferData->ElementDataSize, byteData.size());
			}
		}
		else
		{
			return false;
		}

		return true;
	}


	DirectX12ConstantBuffer::~DirectX12ConstantBuffer()
	{
	}

	bool DirectX12ConstantBuffer::SetData(void* datas, u64 elementSize)
	{
		return false;
	}

	bool DirectX12ConstantBuffer::IsValid() const
	{
		return false;
	}

	void DirectX12ConstantBuffer::SetBufferLoadMethod(EBufferLoadMethod method)
	{
	}

	EBufferLoadMethod DirectX12ConstantBuffer::GetBufferLoadMethod() const
	{
		return EBufferLoadMethod();
	}
	void DirectX12ConstantBuffer::Bind()
	{
	}
	void DirectX12ConstantBuffer::Reset()
	{
	}
	DirectX12StructuredBuffer::~DirectX12StructuredBuffer()
	{
	}
	bool DirectX12StructuredBuffer::SetData(void* datas, u64 elementSize, u64 elementCount)
	{
		return false;
	}
	bool DirectX12StructuredBuffer::IsValid() const
	{
		return false;
	}

	void DirectX12StructuredBuffer::SetBufferLoadMethod(EBufferLoadMethod method)
	{
	}

	EBufferLoadMethod DirectX12StructuredBuffer::GetBufferLoadMethod() const
	{
		return EBufferLoadMethod();
	}

	void DirectX12StructuredBuffer::Bind()
	{
	}

	void DirectX12StructuredBuffer::Reset()
	{
	}


	DirectX12Texture::~DirectX12Texture()
	{
		
	}

	void DirectX12Texture::SetName(const String& name)
	{
		ITexture::SetName(name);
		if (mD3DResource != nullptr)
		{
			mD3DResource->SetName(name.c_str());
			ResourceStateTracker::SetResourceName(mD3DResource.Get(), name);
		}
	}

	TextureID DirectX12Texture::GetTextureID() const
	{
		return GetSRV().ptr;
	}

	const TextureInfo& DirectX12Texture::GetTextureInfo() const
	{
		return mTextureInfo;
	}
	void DirectX12Texture::SetTextureInfo(const TextureInfo& info)
	{
		Reset();

		mTextureInfo = info;
		auto flags = mTextureInfo.Flags;



		D3D12_RESOURCE_FLAGS d3dRscFlags = D3D12_RESOURCE_FLAG_NONE;

		if (flags & ETextureFlags::Allow_RenderTarget)
		{
			d3dRscFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}
		else if (flags & ETextureFlags::Allow_DepthStencil)
		{
			d3dRscFlags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}


		D3D12_RESOURCE_DESC rscDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			ConvertDXGIFormat(mTextureInfo.Format), mTextureInfo.Width, mTextureInfo.Height,
			info.ArraySize, mTextureInfo.MipLevel, 1, 0,
			d3dRscFlags, D3D12_TEXTURE_LAYOUT_UNKNOWN, 0);


		D3D12_CLEAR_VALUE clearValue;
		if (flags & ETextureFlags::Allow_RenderTarget)
		{
			clearValue.Color[0] = info.ClearColor.R;
			clearValue.Color[1] = info.ClearColor.G;
			clearValue.Color[2] = info.ClearColor.B;
			clearValue.Color[3] = info.ClearColor.A;
		}
		else if (flags & ETextureFlags::Allow_DepthStencil)
		{
			clearValue.DepthStencil.Depth = info.ClearDepth;
			clearValue.DepthStencil.Stencil = info.ClearStencil;
		}
		clearValue.Format = ConvertDXGIFormat(info.Format);

		DirectX12API::GetD3DDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&rscDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&clearValue, IID_PPV_ARGS(mD3DResource.GetAddressOf()));
		mD3DResource->SetName(GetName().c_str());
		ResourceStateTracker::RegisterResource(GetName(), mD3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);
	}
	bool DirectX12Texture::IsValid() const
	{
		
		return mD3DResource != nullptr;;
	}

	void DirectX12Texture::Bind()
	{
		//  TODO
		// 고민
		/*
		*	텍스쳐 바인딩

		
		*/
	}

	void DirectX12Texture::Create(const String& name, const TextureInfo& info)
	{
		SetName(name);
		SetTextureInfo(info);
	}

	void DirectX12Texture::Reset()
	{
		if (mD3DResource == nullptr)
		{
			return;
		}
		ResourceStateTracker::UnRegisterResource(mD3DResource.Get());
		mD3DResource.Reset();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Texture::GetRTV() const
	{
		if (IsValid() == false) return { 0 };
		// 렌더 타겟 텍스쳐가 아니라면 0을 뿜어냄
		if ((mTextureInfo.Flags & ETextureFlags::Allow_RenderTarget) == false)
		{
			JG_CORE_ERROR("{0} not supported RenderTarget because does not include  ETextureFlags::Allow_RenderTarget Flag", ws2s(GetName()));
			return { 0 };
		}

		SharedPtr<D3D12_RENDER_TARGET_VIEW_DESC> rtvDesc = CreateRTVDesc(mTextureInfo.Flags);


		u64 hash = 0;
		if (rtvDesc != nullptr)
		{
			hash = std::hash<D3D12_RENDER_TARGET_VIEW_DESC>{}(*rtvDesc);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle = { 0 };
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(mRTVMutex);
			isFind = mRTVs.find(hash) != mRTVs.end();
			if (isFind == true)
			{
				handle = mRTVs[hash].CPU();
			}
		}
		if(isFind == false)
		{
			std::lock_guard<std::shared_mutex> lock(mRTVMutex);
			auto alloc = DirectX12API::RTVAllocate();
			DirectX12API::GetD3DDevice()->CreateRenderTargetView(Get(), rtvDesc.get(), alloc.CPU());
			mRTVs.emplace(hash, std::move(alloc));
			handle = mRTVs[hash].CPU();
		}

		return handle;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Texture::GetDSV() const
	{
		if (IsValid() == false) return { 0 };
		if ((mTextureInfo.Flags & ETextureFlags::Allow_DepthStencil) == false)
		{
			JG_CORE_ERROR("{0} not supported DepthStencil because does not include  ETextureFlags::Allow_DepthStencil Flag", ws2s(GetName()));
			return { 0 };
		}

		SharedPtr<D3D12_DEPTH_STENCIL_VIEW_DESC> dsvDesc = CreateDSVDesc(mTextureInfo.Flags);

		u64 hash = 0;
		if (dsvDesc != nullptr)
		{
			hash = std::hash<D3D12_DEPTH_STENCIL_VIEW_DESC>{}(*dsvDesc);
		}


		D3D12_CPU_DESCRIPTOR_HANDLE handle = { 0 };
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(mDSVMutex);
			isFind = mDSVs.find(hash) != mDSVs.end();
			if (isFind == true)
			{
				handle = mDSVs[hash].CPU();
			}
		}
		if (isFind == false)
		{
			std::lock_guard<std::shared_mutex> lock(mDSVMutex);
			auto alloc = DirectX12API::DSVAllocate();
			DirectX12API::GetD3DDevice()->CreateDepthStencilView(Get(), dsvDesc.get(), alloc.CPU());
			mDSVs.emplace(hash, std::move(alloc));
			handle = mDSVs[hash].CPU();
		}

		return handle;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Texture::GetSRV() const
	{
		if (IsValid() == false) return { 0 };

		SharedPtr<D3D12_SHADER_RESOURCE_VIEW_DESC> srvDesc = CreateSRVDesc(mTextureInfo.Flags);

		u64 hash = 0;
		if (srvDesc != nullptr)
		{
			hash = std::hash<D3D12_SHADER_RESOURCE_VIEW_DESC>{}(*srvDesc);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle = { 0 };
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(mSRVMutex);
			isFind = mSRVs.find(hash) != mSRVs.end();
			if (isFind == true)
			{
				handle = mSRVs[hash].CPU();
			}
		}
		if (isFind == false)
		{
			std::lock_guard<std::shared_mutex> lock(mSRVMutex);
			auto alloc = DirectX12API::CSUAllocate();
			DirectX12API::GetD3DDevice()->CreateShaderResourceView(Get(), srvDesc.get(), alloc.CPU());
			mSRVs.emplace(hash, std::move(alloc));
			handle = mSRVs[hash].CPU();
		}

		return handle;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Texture::GetUAV() const
	{
		if (IsValid() == false) return { 0 };

		SharedPtr<D3D12_UNORDERED_ACCESS_VIEW_DESC> uavDesc = CreateUAVDesc(mTextureInfo.Flags);
		u64 hash = 0;
		if (uavDesc != nullptr)
		{
			hash = std::hash<D3D12_UNORDERED_ACCESS_VIEW_DESC>{}(*uavDesc);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle = { 0 };
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(mUAVMutex);
			isFind = mUAVs.find(hash) != mUAVs.end();
			if (isFind == true)
			{
				handle = mUAVs[hash].CPU();
			}
		}
		if (isFind == false)
		{
			std::lock_guard<std::shared_mutex> lock(mUAVMutex);
			auto alloc = DirectX12API::CSUAllocate();
			DirectX12API::GetD3DDevice()->CreateUnorderedAccessView(Get(), nullptr, uavDesc.get(), alloc.CPU());
			mUAVs.emplace(hash, std::move(alloc));
			handle = mUAVs[hash].CPU();
		}

		return handle;
	}

	SharedPtr<D3D12_RENDER_TARGET_VIEW_DESC> DirectX12Texture::CreateRTVDesc(ETextureFlags flag) const
	{
		return nullptr;
	}

	SharedPtr<D3D12_DEPTH_STENCIL_VIEW_DESC> DirectX12Texture::CreateDSVDesc(ETextureFlags flag) const
	{
		return nullptr;
	}

	SharedPtr<D3D12_SHADER_RESOURCE_VIEW_DESC> DirectX12Texture::CreateSRVDesc(ETextureFlags flag) const
	{
		return nullptr;
	}

	SharedPtr<D3D12_UNORDERED_ACCESS_VIEW_DESC> DirectX12Texture::CreateUAVDesc(ETextureFlags flag) const
	{
		return nullptr;
	}
	







}
