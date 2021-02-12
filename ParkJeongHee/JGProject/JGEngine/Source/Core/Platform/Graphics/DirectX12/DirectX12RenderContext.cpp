#include "pch.h"
#include "DirectX12RenderContext.h"
#include "DirectX12API.h"
#include "Utill/CommandQueue.h"
#include "Utill/CommandList.h"


namespace JG
{

	bool DirectX12RenderContext::Init(const RenderContextSettings& settings)
	{
		mSettings = settings;
		mSwapChain = CreateDXGISwapChain(
			(HWND)mSettings.Handle,
			DirectX12API::GetDXGIFactory(),
			DirectX12API::GetGraphicsCommandQueue()->Get(),
			ConvertDirectX12TextureFormat(mSettings.Format),
			mSettings.Width, mSettings.Height,
			(u32)DirectX12API::GetFrameBufferCount());


		if (mSwapChain == nullptr)
		{
			JG_CORE_CRITICAL("Failed Create SwapChain in DirectX12RenderContext");
			return false;
		}
		mBackBuffers.resize(DirectX12API::GetFrameBufferCount());
		mRTVs.resize(DirectX12API::GetFrameBufferCount());
		for (u64 i = 0; i < DirectX12API::GetFrameBufferCount(); ++i)
		{
			mRTVs[i] = DirectX12API::RTVAllocate();
			mSwapChain->GetBuffer((u32)i, IID_PPV_ARGS(mBackBuffers[i].GetAddressOf()));
			DirectX12API::GetD3DDevice()->CreateRenderTargetView(mBackBuffers[i].Get(), nullptr, mRTVs[i].CPU());
		}
		return true;
	}

	void DirectX12RenderContext::Update()
	{
		u32 index = mSwapChain->GetCurrentBackBufferIndex();
		auto& backBuffer = mBackBuffers[index];


		auto commandQueue = DirectX12API::GetGraphicsCommandQueue();
		auto commandList = commandQueue->RequestCommandList(JG_U32_MAX);

		commandList->Get()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));


		// TODO
		// 설정 값으로
		f32 clearColor[4] = { mSettings.ClearColor.R,mSettings.ClearColor.G, mSettings.ClearColor.B, mSettings.ClearColor.A };
		commandList->Get()->ClearRenderTargetView(mRTVs[index].CPU(), clearColor, 0, nullptr);


		commandList->Get()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		//if (m_FinalTexture.IsValid())
		//{
		//	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		//		backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST));
		//	DXCommand::TransitionBarrier(cmdKey, m_FinalTexture, D3D12_RESOURCE_STATE_COPY_SOURCE);


		//	cmdList->CopyResource(backBuffer.Get(), m_FinalTexture.GetD3DPtr());

		//	DXCommand::TransitionBarrier(cmdKey, m_FinalTexture, D3D12_RESOURCE_STATE_COMMON);
		//}
		//commandList->Get()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		//	backBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT));
	}

	bool DirectX12RenderContext::Present()
	{
		HRESULT hResult = mSwapChain->Present(0, 0);
		return SUCCEEDED(hResult);
	}
	void DirectX12RenderContext::SubmitTexture(const Texture& texture)
	{

	}

	void DirectX12RenderContext::Resize(u32 width, u32 height)
	{
		if (mSettings.Width == width && mSettings.Height == height) return;


		for (u64 i = 0; i < DirectX12API::GetFrameBufferCount(); ++i)
		{
			mBackBuffers[i].Reset();
		}

		DXGI_SWAP_CHAIN_DESC1 desc;
		mSwapChain->GetDesc1(&desc);
		desc.Width  = std::max<u32>(1, width);
		desc.Height = std::max<u32>(1, height);

		mSettings.Width = desc.Width;
		mSettings.Height = desc.Height;
		mSwapChain->ResizeBuffers((u32)DirectX12API::GetFrameBufferCount(), desc.Width, desc.Height, desc.Format, desc.Flags);

		for (u64 i = 0; i < DirectX12API::GetFrameBufferCount(); ++i)
		{
			mSwapChain->GetBuffer((u32)i, IID_PPV_ARGS(mBackBuffers[i].GetAddressOf()));
			DirectX12API::GetD3DDevice()->CreateRenderTargetView(mBackBuffers[i].Get(), nullptr, mRTVs[i].CPU());
		}

	}

	const RenderContextSettings& DirectX12RenderContext::GetSettings() const
	{
		return mSettings;
	}




}