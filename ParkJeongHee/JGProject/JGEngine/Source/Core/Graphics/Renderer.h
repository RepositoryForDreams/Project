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
	class IVertexBuffer;
	class IIndexBuffer;


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




	class Renderer2D
	{
		struct QuadVertex
		{
			JVector3 Pos;
			JVector2 Tex;
			Color _Color = Color::White();
			i32 TextureIndex = 0;
			QuadVertex() = default;
			QuadVertex(const JVector3& pos, const JVector2& tex) : Pos(pos), Tex(tex) {}
			QuadVertex(const JVector3& pos, const JVector2& tex, const Color& color, i32 textureIndex)
				: Pos(pos), Tex(tex), _Color(color), TextureIndex(textureIndex) {}
		};

	private:
		static const u32 MaxQuadCount    = 1200;
		static const u32 MaxVertexCount  = MaxQuadCount * 4;
		static const u32 MaxIndexCount   = MaxQuadCount * 6;
		static const u32 MaxTextureCount = 64;

		static const u32 QuadVertexCount = 4;
		static const u32 QuadIndexCount  = 6;
	private:
		JVector3 mStandardQuadPosition[4];
		JVector2 mStandardQuadTexcoord[4];

		SharedPtr<IMesh> mQuadMesh; 
		SharedPtr<IVertexBuffer> mQuadVBuffer;
		SharedPtr<IIndexBuffer>  mQuadIBuffer;

		SharedPtr<IShader>   mStandard2DShader;
		SharedPtr<IMaterial> mStandard2DMaterial; // Bind
		List<QuadVertex> mVertices;
		List<u32>        mIndices;
		List<SharedPtr<ITexture>> mTextureArray;
		SharedPtr<ITexture> mWhiteTexture;

		u64 mQuadCount = 0;
		u64 mTextureCount = 0;
	public:
		Renderer2D();
		~Renderer2D();
	public:
		bool Begin(SharedPtr<Camera> camera);
		bool Begin(const JVector2& resolutoin, const JMatrix& viewProj, SharedPtr<ITexture> targetTexture);
		void End();

		void DrawCall(const JMatrix& transform, SharedPtr<ITexture> texture, const Color& color);
		void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<ITexture> texture, const Color& color);
		void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, const Color& color);
		void DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<ITexture> texture);
		void DrawCall(const JVector2& Pos, const JVector2& Size, const Color& color);
		void DrawCall(const JVector2& Pos, const JVector2& Size, SharedPtr<ITexture> texture);
	private:
		void StartBatch();
		void NextBatch();
	};
}

