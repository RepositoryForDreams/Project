﻿#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"

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
	class IMaterial;
	class IMesh;
	class IComputeBuffer;
	class IComputer;
	struct TextureInfo;
	struct FrameBufferInfo;
	class IGraphicsAPI 
	{
	public:
		virtual EGraphicsAPI GetAPI() const = 0;
	public:
		friend class Application;
		virtual bool Create() = 0;
		virtual void Destroy() = 0;

		
		friend class Renderer2D;
		friend class Renderer;
		virtual void Begin() = 0;
		virtual void End()	 = 0;
		virtual void Flush() = 0;
		//
		virtual void SetViewports(const List<Viewport>& viewPorts) = 0;
		virtual void SetScissorRects(const List<ScissorRect>& scissorRects) = 0;
		virtual void ClearRenderTarget(const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture) = 0;
		virtual void SetRenderTarget(const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture)   = 0;
		virtual void DrawIndexed(u32 indexCount, u32 instancedCount = 1, u32 startIndexLocation = 0, u32 startVertexLocation = 0, u32 startInstanceLocation = 0) = 0;
		// State

		virtual void SetDepthStencilState(EDepthStencilStateTemplate _template) = 0;
		virtual void SetBlendState(u32 renderTargetSlot, EBlendStateTemplate _template) = 0;
		virtual void SetRasterizerState(ERasterizerStateTemplate _template) = 0;


		// Create Resource
		friend IVertexBuffer;
		friend IIndexBuffer;
		friend IShader;
		friend IFrameBuffer;
		friend ITexture;
		friend IMesh;
		friend IMaterial;
		friend IComputeBuffer;
		friend IComputer;

		virtual SharedPtr<IFrameBuffer>   CreateFrameBuffer(const FrameBufferInfo& settings) = 0;
		virtual SharedPtr<IVertexBuffer>  CreateVertexBuffer(const String& name, EBufferLoadMethod method) = 0;
		virtual SharedPtr<IIndexBuffer>   CreateIndexBuffer(const String& name, EBufferLoadMethod method) = 0;
		virtual SharedPtr<IComputeBuffer> CreateComputeBuffer(const String& name, u64 btSize) = 0;
		virtual SharedPtr<IComputer>      CreateComputer(const String& name, SharedPtr<IShader> shader) = 0;
		virtual SharedPtr<IShader>        CreateShader(const String& name, const String& sourceCode, EShaderFlags flags) = 0;
		virtual SharedPtr<IMaterial>	  CreateMaterial(const String& name, SharedPtr<IShader> shader) = 0;
		virtual SharedPtr<IMesh>          CreateMesh(const String& name) = 0;
		virtual SharedPtr<ITexture>       CreateTexture(const String& name, const TextureInfo& info) = 0;
		virtual SharedPtr<ITexture>       CreateTextureFromFile(const String& path) = 0;
	public:
		virtual void ClearTexture(SharedPtr<ITexture> texture);
	public:
		static UniquePtr<IGraphicsAPI> Create(EGraphicsAPI api);
		
	};


}















