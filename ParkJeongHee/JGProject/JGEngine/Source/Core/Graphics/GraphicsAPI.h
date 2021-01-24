#pragma once
#include "JGCore.h"
#include "Resource.h"
#include "Shader.h"







namespace JG
{
	class IGraphicsAPI 
	{
	public:
		virtual bool Create()  = 0;
		virtual void Destroy() = 0;
		virtual EGraphicsAPI GetAPI() const = 0;

	public:
		static UniquePtr<IGraphicsAPI> Create(EGraphicsAPI api);
	};


	
}















