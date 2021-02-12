#pragma once
#include "JGCore.h"

namespace JG
{
	class IRenderContext;
	class InputLayout;
	class IShader;
	class ITexture;
	class Renderer3D
	{
	public:


		static void SubmitRenderContext(SharedPtr<IRenderContext> context);

	};
	// 같은 Shader 라면 
	// 차피 VertexArray 다
	// VertexArray에 Material, Texture 바인딩 시켜서 제출하자.

	class Renderer2D
	{
	public:
		// ClearColor, Camera정보, 
		// RenderTarget Texture 생성
		// 
		static void Begin();
		//static void DrawQuad();//Vec, Rotation, Scale//, )


		// 박아야할 데이터
		// InputLayout
		// Shader
		// 

		// 가변 데이터
		// Vertex, Index
		// Material
		// Texture



		// 그린 텍스쳐 반환
		static SharedPtr<ITexture> End();
		static void SubmitRenderContext(SharedPtr<IRenderContext> context);
		
	};
}

