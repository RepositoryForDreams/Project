﻿#pragma once
#include "JGCore.h"


namespace JG
{
	class IRenderContext;
	class IGraphicsAPI 
	{
	public:
		virtual EGraphicsAPI GetAPI() const = 0;
	protected:
		friend class Application;
		virtual bool Create() = 0;
		virtual void Destroy() = 0;


		friend class Renderer;
		virtual void Begin() = 0;
		virtual void End()	 = 0;
		virtual void Flush() = 0;
		virtual void SubmitRenderContext(SharedPtr<IRenderContext> renderContext) = 0;
	public:
		static UniquePtr<IGraphicsAPI> Create(EGraphicsAPI api);
	};
}















