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
	// 같은 Shader 라면 
	// 차피 VertexArray 다
	// VertexArray에 Material, Texture 바인딩 시켜서 제출하자.

	class Renderer2D
	{
		friend class Renderer;
		static bool Create();
		static void Destroy();
	public:
		struct Statistics
		{
			u32 DrawCalls = 0;
			u32 QuadCount = 0;

			u32 GetTotalVertexCount() const { return QuadCount * 4; }
			u32 GetTotalIndexCount() const { return QuadCount * 6; }
		};
	public:
		// Standard Material
		static void DrawCall(const JMatrix& transform, SharedPtr<ITexture> texture, const Color& color);
		static void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<ITexture> texture , const Color& color);
		static void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, const Color& color);
		static void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<ITexture> texture);
		static void DrawCall(const JVector2& Pos, const JVector2& Size, const Color& color);
		static void DrawCall(const JVector2& Pos, const JVector2& Size, SharedPtr<ITexture> texture);
		// Custom Material
		//static void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<IMaterial> material);
		static const Statistics& GetStats();
	private:

		static void StartBatch();
		static void NextBatch();
	};
}

