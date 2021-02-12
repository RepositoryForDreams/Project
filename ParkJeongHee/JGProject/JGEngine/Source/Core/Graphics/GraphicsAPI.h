#pragma once
#include "JGCore.h"


namespace JG
{
	/* 넣어야할 설정 값
	Renderer2D Shader

	
	*/
	enum class ShaderFlags;
	class IVertexBuffer;
	class IIndexBuffer;
	class IShader;
	class IRenderContext;
	
	struct RenderContextSettings;
	class IGraphicsAPI 
	{
	public:
		virtual EGraphicsAPI GetAPI() const = 0;
	protected:
		friend class Application;
		virtual bool Create() = 0;
		virtual void Destroy() = 0;

		
		friend class Renderer2D;
		friend class Renderer3D;
		virtual void Begin() = 0;
		virtual void End()	 = 0;
		virtual void Flush() = 0;
		virtual void SubmitRenderContext(SharedPtr<IRenderContext> renderContext) = 0;

		// Renderer2D  Impl
		virtual void Renderer2D_Begin_Impl() = 0;
		virtual void Renderer2D_End_Impl()   = 0;





		// Renderer3D Impl



		// Create Resource
		friend IVertexBuffer;
		friend IIndexBuffer;
		friend IShader;
		friend IRenderContext;
		virtual SharedPtr<IRenderContext> CreateRenderContext(const RenderContextSettings& settings) = 0;
		virtual SharedPtr<IVertexBuffer>  CreateVertexBuffer(String name, void* datas, u64 elementSize, u64 elementCount) = 0;
		virtual SharedPtr<IIndexBuffer>   CreateIndexBuffer(String name, u32* datas, u32 count) = 0;
		virtual SharedPtr<IShader>        CreateShader(const String& sourceCode, ShaderFlags flags, const String& error) = 0;
	public:
		static UniquePtr<IGraphicsAPI> Create(EGraphicsAPI api);
	};
}















