#pragma once
#include "JGCore.h"

namespace JG
{
	class IRenderContext;
	class Renderer
	{
	private:
		friend class Application;
		static void Begin();
		static void End();
	
	public:
		static void Flush();
		static void SubmitRenderContext(SharedPtr<IRenderContext> context);
	};

	class Renderer3D
	{


	};


	class Renderer2D
	{
	public:

		
	};
}

