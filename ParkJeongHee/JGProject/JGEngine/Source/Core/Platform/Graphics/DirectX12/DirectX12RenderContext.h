#pragma once
#include "Graphics/RenderContext.h"
#include "Utill/DirectX12Helper.h"
#include "Utill/DescriptorAllocator.h"
namespace JG
{
	class DirectX12RenderContext : public IRenderContext
	{
	private:
		std::vector<ComPtr<ID3D12Resource>> mBackBuffers;
		std::vector<DescriptorAllocation>   mRTVs;
		ComPtr<IDXGISwapChain4>				mSwapChain;
		RenderContextSettings mSettings;
	public:
		virtual bool Init(const RenderContextSettings& settings) override;
		virtual void Update() override;
		virtual bool Present() override;


		virtual void SubmitTexture(const Texture& texture) override;
		virtual void Resize(u32 width, u32 height)		   override;
		virtual const RenderContextSettings& GetSettings() const override;
	};





}