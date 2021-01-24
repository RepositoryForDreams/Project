#pragma once
#include "Graphics/GraphicsAPI.h"





namespace JG
{
	class DirectX12API : public IGraphicsAPI
	{

	public:
		virtual bool Create() override;
		virtual void Destroy() override;
		virtual EGraphicsAPI GetAPI()const override;
	};
}
