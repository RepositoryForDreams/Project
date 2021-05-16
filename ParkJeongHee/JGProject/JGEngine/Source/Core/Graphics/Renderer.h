#pragma once
#include "JGCore.h"

namespace JG
{
	class IRenderContext;
	class InputLayout;
	class IShader;
	class ITexture;
	class IMaterial;
	class IMesh;
	class Application;
	class Camera;



	class Renderer
	{
		friend Application;
	public:
		struct Statistics
		{
			u32 DrawCalls = 0;
		};
	private:
		static bool Create();
		static void Destroy();
	public:
		static bool Begin(SharedPtr<Camera> camera);
		static void DrawCall(SharedPtr<IMesh> mesh, SharedPtr<IMaterial> material);
		static void End();
	};
}

