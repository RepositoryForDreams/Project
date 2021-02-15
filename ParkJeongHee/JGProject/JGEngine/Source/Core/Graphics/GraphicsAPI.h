#pragma once
#include "JGCore.h"


namespace JG
{
	/* 넣어야할 설정 값
	Renderer2D Shader

	
	*/
	enum class EShaderFlags;
	class IVertexBuffer;
	class IIndexBuffer;
	class IShader;
	class IRenderContext;
	
	struct RenderContextSettings;
	class IGraphicsAPI 
	{
	public:
		virtual EGraphicsAPI GetAPI() const = 0;
		virtual void SubmitRenderContext(SharedPtr<IRenderContext> renderContext) = 0;
	protected:
		friend class Application;
		virtual bool Create() = 0;
		virtual void Destroy() = 0;

		
		friend class Renderer2D;
		friend class Renderer3D;
		virtual void Begin() = 0;
		virtual void End()	 = 0;
		virtual void Flush() = 0;



		// Create Resource
		friend IVertexBuffer;
		friend IIndexBuffer;
		friend IShader;
		friend IRenderContext;
		virtual SharedPtr<IRenderContext> CreateRenderContext(const RenderContextSettings& settings) = 0;
		virtual SharedPtr<IVertexBuffer>  CreateVertexBuffer(const String& name, void* datas, u64 elementSize, u64 elementCount) = 0;
		virtual SharedPtr<IIndexBuffer>   CreateIndexBuffer(const String& name, u32* datas, u64 count) = 0;
		virtual SharedPtr<IShader>        CreateShader(const String& sourceCode, EShaderFlags flags, const String& error) = 0;
	public:
		static UniquePtr<IGraphicsAPI> Create(EGraphicsAPI api);
	};
}















