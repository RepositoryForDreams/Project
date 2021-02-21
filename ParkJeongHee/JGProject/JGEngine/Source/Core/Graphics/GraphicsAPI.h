#pragma once
#include "JGCore.h"


namespace JG
{
	/* 넣어야할 설정 값
	Renderer2D Shader

	
	*/
	enum class EShaderFlags;
	class ITexture;
	class IVertexBuffer;
	class IIndexBuffer;
	class IShader;
	class IFrameBuffer;
	struct TextureInfo;
	struct FrameBufferInfo;
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
		//

		virtual void ClearRenderTarget(const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture) = 0;
		virtual void SetRenderTarget(const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture)   = 0;




		// Create Resource
		friend IVertexBuffer;
		friend IIndexBuffer;
		friend IShader;
		friend IFrameBuffer;
		friend ITexture;
		virtual SharedPtr<IFrameBuffer>   CreateFrameBuffer(const FrameBufferInfo& settings) = 0;
		virtual SharedPtr<IVertexBuffer>  CreateVertexBuffer(const String& name, void* datas, u64 elementSize, u64 elementCount) = 0;
		virtual SharedPtr<IIndexBuffer>   CreateIndexBuffer(const String& name, u32* datas, u64 count) = 0;
		virtual SharedPtr<IShader>        CreateShader(const String& sourceCode, EShaderFlags flags, const String& error) = 0;
		virtual SharedPtr<ITexture>       CreateTexture(const String& name, const TextureInfo& info) = 0;
		virtual SharedPtr<ITexture>       CreateTextureFromFile(const String& path) = 0;
	public:
		static UniquePtr<IGraphicsAPI> Create(EGraphicsAPI api);
	};
}















