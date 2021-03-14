#pragma once

#include "JGCore.h"
#include "GraphicsDefine.h"
#include <shared_mutex>
namespace JG
{
	/* Shader
	 * HLSL 컴 파일 하고 모시기 하고하는곳
	 * Compile 할때 코드 분석
	 *
	 * 
	 */


	

	/* Shader 자체
	
	*/
	class IComputeBuffer;
	class ITexture;
	class IShader
	{
	protected:
		virtual bool Compile(const String& sourceCode, EShaderFlags flags, String* error) = 0;
		virtual bool Bind() = 0;
	public:
		virtual void SetName(const String& name) = 0;
		virtual const String& GetName() const    = 0;
		virtual EShaderFlags  GetFlags() const = 0;
	public:
		static SharedPtr<IShader> Create(const String& name, const String& sourceCode, EShaderFlags flags);
	};

	class IComputeShader
	{
	protected:
		virtual bool Compile(const String& sourceCode, EShaderFlags flags, String* error) = 0;
		virtual bool Bind() = 0;
	public:
		virtual void SetName(const String& name) = 0;
		virtual const String& GetName() const = 0;
	public:
		virtual bool SetComputeBuffer(SharedPtr<IComputeBuffer> computeBuffer) = 0;
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
	public:
		virtual void Dispatch(u32 groupX, u32 groupY, u32 groupZ) = 0;
	public:
		static SharedPtr<IComputeShader> Create(const String& name, const String& sourceCode);
	};



	class ShaderLibrary : public GlobalSingleton<ShaderLibrary>
	{
	private:
		Dictionary<String, SharedPtr<IShader>> mShaders;
		std::shared_mutex mMutex;
	public:
		static void RegisterShader(SharedPtr<IShader> shader);
		static SharedPtr<IShader> Get(const String& name);
	};



	
	
}


