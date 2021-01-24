#pragma once
#include "Graphics/GraphicsAPI.h"




struct ID3D12Device;
struct IDXGIFactory4;
namespace JG
{


	
	class DirectX12API : public IGraphicsAPI
	{
	public:
		virtual bool Create() override;
		virtual void Destroy() override;
		virtual EGraphicsAPI GetAPI()const override;


	public:
		static IDXGIFactory4* GetDXGIFactory();
		static ID3D12Device*  GetD3DDevice();
	};
}
