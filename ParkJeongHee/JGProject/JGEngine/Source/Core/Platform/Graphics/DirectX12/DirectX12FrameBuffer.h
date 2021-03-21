#pragma once
#include "Graphics/FrameBuffer.h"
#include "Utill/DirectX12Helper.h"
#include "Utill/DescriptorAllocator.h"
namespace JG
{
	using DirectX12FrameBufferUpdateCallBack = std::function<D3D12_RESOURCE_STATES(ID3D12GraphicsCommandList*, ID3D12Resource*, D3D12_CPU_DESCRIPTOR_HANDLE,  D3D12_RESOURCE_STATES)>;
	class DirectX12FrameBuffer : public IFrameBuffer
	{
	private:
		List<ComPtr<ID3D12Resource>> mBackBuffers;
		List<DescriptorAllocation>   mRTVs;
		List<DirectX12FrameBufferUpdateCallBack> mUpdateCallBackList;
		ComPtr<IDXGISwapChain4>	     mSwapChain;
		WeakPtr<ITexture>            mSubmittedTexture;

		FrameBufferInfo mInfo;
	protected:
		friend class DirectX12API;
		virtual bool Init(const FrameBufferInfo& info) override;
		virtual void Update() override;
		virtual bool Present() override;
	public:
		virtual void SubmitTexture(SharedPtr<ITexture> texture) override;
		virtual void Resize(u32 width, u32 height)		        override;
		virtual const FrameBufferInfo& GetInfo() const          override;
		void AddUpdateCallBack(const DirectX12FrameBufferUpdateCallBack& callBack);
	public:
		void Reset();

	};





}