#pragma once
#include "JGCore.h"


namespace JG
{
	class IGraphicsAPI 
	{
	public:
		virtual bool Create()  = 0;
		virtual void Destroy() = 0;
		virtual EGraphicsAPI GetAPI() const = 0;
	protected:
		friend class Renderer;
		virtual void Begin() = 0;
		virtual void End()	 = 0;
		virtual void Flush() = 0;
	public:
		static UniquePtr<IGraphicsAPI> Create(EGraphicsAPI api);
	};
}















