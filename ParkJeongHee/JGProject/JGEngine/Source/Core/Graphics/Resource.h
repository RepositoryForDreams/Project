#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"
#include "InputLayout.h"
namespace JG
{
	class ITexture;
	class IShader;
	class IResource
	{
	private:
		String mName = TT("IResource");
	public:
		IResource() = default;
		virtual ~IResource() = default;
		virtual void SetName(const String& name)
		{
			mName = name;
		}
		const String& GetName() const
		{
			return mName;
		}
	public:
		virtual bool IsValid() const = 0;
	private:
		IResource(const IResource&) = delete;
		const IResource& operator=(const IResource&) = delete;
	};

	class Buffer : public IResource {};

	class IVertexBuffer : public Buffer
	{
	public:
		virtual ~IVertexBuffer() = default;
	protected:
		virtual void SetBufferLoadMethod(EBufferLoadMethod method) = 0;
	public:
		virtual bool SetData(void* datas, u64 elementSize, u64 elementCount) = 0;
		virtual EBufferLoadMethod GetBufferLoadMethod() const = 0;

		static SharedPtr<IVertexBuffer> Create(String name, EBufferLoadMethod method);
	};

	class IIndexBuffer : public Buffer
	{
	public:
		virtual ~IIndexBuffer() = default;
	protected:
		virtual void SetBufferLoadMethod(EBufferLoadMethod method) = 0;
	public:
		virtual bool SetData(u32* datas, u64 count) = 0;
		virtual EBufferLoadMethod GetBufferLoadMethod() const = 0;


		static SharedPtr<IIndexBuffer> Create(String name, EBufferLoadMethod method);
	};
	


	class IComputeBuffer : public Buffer
	{
	public:
		virtual bool SetFloat(const String& name, float value) = 0;
		virtual bool SetFloat2(const String& name, const JVector2& value) = 0;
		virtual bool SetFloat3(const String& name, const JVector3& value) = 0;
		virtual bool SetFloat4(const String& name, const JVector4& value) = 0;
		virtual bool SetInt(const String& name, i32 value) = 0;
		virtual bool SetInt2(const String& name, const JVector2Int& value) = 0;
		virtual bool SetInt3(const String& name, const JVector3Int& value) = 0;
		virtual bool SetInt4(const String& name, const JVector4Int& value) = 0;
		virtual bool SetUint(const String& name, u32 value) = 0;
		virtual bool SetUint2(const String& name, const JVector2Uint& value) = 0;
		virtual bool SetUint3(const String& name, const JVector3Uint& value) = 0;
		virtual bool SetUint4(const String& name, const JVector4Uint& value) = 0;
		virtual bool SetFloat4x4(const String& name, const JMatrix& value) = 0;
		virtual bool SetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture> texture) = 0;

		virtual bool SetFloatArray(const String& name, const List<float>& value) = 0;
		virtual bool SetFloat2Array(const String& name, const List<JVector2>& value) = 0;
		virtual bool SetFloat3Array(const String& name, const List<JVector3>& value) = 0;
		virtual bool SetFloat4Array(const String& name, const List<JVector4>& value) = 0;
		virtual bool SetIntArray(const String& name, const List<i32>& value) = 0;
		virtual bool SetInt2Array(const String& name, const List<JVector2Int>& value) = 0;
		virtual bool SetInt3Array(const String& name, const List<JVector3Int>& value) = 0;
		virtual bool SetInt4Array(const String& name, const List<JVector4Int>& value) = 0;
		virtual bool SetUintArray(const String& name, const List<u32>& value) = 0;
		virtual bool SetUint2Array(const String& name, const List<JVector2Uint>& value) = 0;
		virtual bool SetUint3Array(const String& name, const List<JVector3Uint>& value) = 0;
		virtual bool SetUint4Array(const String& name, const List<JVector4Uint>& value) = 0;
		virtual bool SetFloat4x4Array(const String& name, const List<JMatrix>& value) = 0;
		virtual bool SetStructDataArray(const String& name, void* datas, u64 elementCount, u64 elementSize) = 0;

		virtual bool GetFloat(const String& name, float* out_value) = 0;
		virtual bool GetFloat2(const String& name, JVector2* out_value) = 0;
		virtual bool GetFloat3(const String& name, JVector3* out_value) = 0;
		virtual bool GetFloat4(const String& name, JVector4* out_value) = 0;
		virtual bool GetInt(const String& name, i32* out_value) = 0;
		virtual bool GetInt2(const String& name, JVector2Int* value) = 0;
		virtual bool GetInt3(const String& name, JVector3Int* value) = 0;
		virtual bool GetInt4(const String& name, JVector4Int* value) = 0;
		virtual bool GetUint(const String& name, u32* value) = 0;
		virtual bool GetUint2(const String& name, JVector2Uint* value) = 0;
		virtual bool GetUint3(const String& name, JVector3Uint* value) = 0;
		virtual bool GetUint4(const String& name, JVector4Uint* value) = 0;
		virtual bool GetFloat4x4(const String& name, JMatrix* out_value) = 0;
		virtual bool GetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture>* out_value) = 0;


		virtual bool GetFloatArray(const String& name, List<float>* out_value) = 0;
		virtual bool GetFloat2Array(const String& name, List<JVector2>* out_value) = 0;
		virtual bool GetFloat3Array(const String& name, List<JVector3>* out_value) = 0;
		virtual bool GetFloat4Array(const String& name, List<JVector4>* out_value) = 0;
		virtual bool GetIntArray(const String& name, List<i32>* out_value) = 0;
		virtual bool GetInt2Array(const String& name, List<JVector2Int>* out_value) = 0;
		virtual bool GetInt3Array(const String& name, List<JVector3Int>* out_value) = 0;
		virtual bool GetInt4Array(const String& name, List<JVector4Int>* out_value) = 0;
		virtual bool GetUintArray(const String& name, List<u32>* out_value) = 0;
		virtual bool GetUint2Array(const String& name, List<JVector2Uint>* out_value) = 0;
		virtual bool GetUint3Array(const String& name, List<JVector3Uint>* out_value) = 0;
		virtual bool GetUint4Array(const String& name, List<JVector4Uint>* out_value) = 0;
		virtual bool GetFloat4x4Array(const String& name, List<JMatrix>* out_value)   = 0;


		virtual bool GetData(const String& name, void* data) = 0;
	public:
		virtual bool IsCompelete() const = 0;
	public:
		static SharedPtr<IComputeBuffer> Create(const String& name, SharedPtr<IShader> shader);
	};
	
	
	class ITexture : public IResource
	{
	public:
		ITexture() = default;
		virtual ~ITexture() = default;
	public:
		virtual TextureID          GetTextureID()   const = 0;
		virtual const TextureInfo& GetTextureInfo() const = 0;
		virtual void  SetTextureInfo(const TextureInfo& info) = 0;
		virtual void  Bind() = 0;
	private:
		ITexture(const ITexture& texture) = delete;
		const ITexture& operator=(const ITexture& texture) = delete;
	public:
		static SharedPtr<ITexture> Create(const String& name, const TextureInfo& info);
		static SharedPtr<ITexture> CreateFromFile(const String& path);
	};
}

