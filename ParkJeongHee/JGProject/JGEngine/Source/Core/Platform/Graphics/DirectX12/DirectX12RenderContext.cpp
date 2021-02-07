#include "pch.h"
#include "DirectX12RenderContext.h"
#include "DirectX12API.h"
#include "Utill/CommandQueue.h"

namespace JG
{



	void DirectX12RenderContext::SubmitTexture(const Texture& texture)
	{
	}

	void DirectX12RenderContext::Resize(u32 width, u32 height)
	{
	}



	bool DirectX12RenderContext::Init(const RenderContextSettings& settings)
	{
		mSwapChain = CreateDXGISwapChain(
			(HWND)settings.Handle,
			DirectX12API::GetDXGIFactory(),
			DirectX12API::GetGraphicsCommandQueue()->Get(),
			ConvertDirectX12TextureFormat(settings.Format),
			settings.Width, settings.Height,
			DirectX12API::GetFrameBufferCount());


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
			mSwapChain->GetBuffer(i, IID_PPV_ARGS(mBackBuffers[i].GetAddressOf()));
			DirectX12API::GetD3DDevice()->CreateRenderTargetView(mBackBuffers[i].Get(), nullptr, mRTVs[i].CPU());
		}



		return true;
	}


	void DirectX12RenderContext::Destroy()
	{
	}

}