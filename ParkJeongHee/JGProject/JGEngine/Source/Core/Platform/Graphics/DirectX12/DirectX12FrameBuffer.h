#pragma once
#include "Graphics/FrameBuffer.h"
#include "Utill/DirectX12Helper.h"
#include "Utill/DescriptorAllocator.h"
namespace JG
{
	class DirectX12FrameBuffer : public IFrameBuffer
	{
	private:
		List<ComPtr<ID3D12Resource>> mBackBuffers;
		List<DescriptorAllocation>   mRTVs;
		ComPtr<IDXGISwapChain4>	     mSwapChain;
		WeakPtr<ITexture>            mSubmittedTexture;
		FrameBufferInfo mInfo;
	protected:
		friend class DirectX12API;
		virtual bool Init(const FrameBufferInfo& info) override;
		virtual void Update() override;
		virtual bool Present() override;
	public:
		// 이것만 노출
		virtual void SubmitTexture(SharedPtr<ITexture> texture) override;
		virtual void Resize(u32 width, u32 height)		   override;
		virtual const FrameBufferInfo& GetInfo() const override;
	};





}