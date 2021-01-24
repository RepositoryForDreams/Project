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
		 // 여기서 커맨드 리스트를 가져 온다.
		virtual void Begin() = 0;


		// 커맨드 큐에 집어 넣는다.
		virtual void End()   = 0;
	};


	class IRenderer2D
	{
	public:
		// BeginDraw(카메라, 타겟이될 텍스쳐)
		// 
		virtual void Begin() = 0;
		virtual void End()   = 0;

		// DrawQuad(위치, 회전, 스케일, 텍스쳐,색,  Flag)
		virtual void DrawQuad() = 0;
		// DrawLine(시작접, 끝점, Flag)
		virtual void DrawLine() = 0;
		
	};
}

