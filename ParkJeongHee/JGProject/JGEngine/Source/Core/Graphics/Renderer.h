#pragma once
#include "JGCore.h"

namespace JG
{
	class IRenderContext;
	class InputLayout;
	class IShader;
	class ITexture;
	class IMaterial;
	class Application;
	class Camera;
	class Renderer3D
	{
	public:


	};
	// 같은 Shader 라면 
	// 차피 VertexArray 다
	// VertexArray에 Material, Texture 바인딩 시켜서 제출하자.

	class Renderer2D
	{
		friend Application;
		static bool Create();
		static void Destroy();
	public:
		// Begin에 카메라
		static bool Begin(SharedPtr<Camera> camera, SharedPtr<ITexture> renderTexture);
		// Standard Material
		static void DrawCall(const JMatrix& transform, SharedPtr<ITexture> texture, const Color& color);
		static void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<ITexture> texture , const Color& color);
		static void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, const Color& color);
		static void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<ITexture> texture);
		static void DrawCall(const JVector2& Pos, const JVector2& Size, const Color& color);
		static void DrawCall(const JVector2& Pos, const JVector2& Size, SharedPtr<ITexture> texture);
		// Custom Material
		//static void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<IMaterial> material);

		static void End();
	private:
		static void StartBatch();
		static void NextBatch();
	};
}

