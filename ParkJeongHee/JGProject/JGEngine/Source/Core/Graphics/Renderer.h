#pragma once
#include "JGCore.h"
#include "Resource.h"
#include "Shader.h"

namespace JG
{
	class IRenderer
	{
	public:
		// BeginDraw(카메라, 타겟이될 텍스쳐)
		virtual void BeginDraw() = 0;
		virtual void EndDraw()   = 0;

		
		virtual void Bind(const ITexture& texture)      = 0;
		virtual void Bind(const IVertexBuffer& vBuffer) = 0;
		virtual void Bind(const IIndexBuffer& iBuffer)  = 0;
	};


	class IRenderer2D
	{
	public:
		// BeginDraw(카메라, 타겟이될 텍스쳐)
		virtual void BeginDraw() = 0;
		virtual void EndDraw()   = 0;

		// DrawQuad(위치, 회전, 스케일, 텍스쳐,색,  Flag)
		virtual void DrawQuad() = 0;
		// DrawLine(시작접, 끝점, Flag)
		virtual void DrawLine() = 0;
		
	};
}

