#pragma once
#include "JGCore.h"






namespace JG
{
#define MAX_RENDERTARGET 8
	enum class ETextureFormat
	{
		None,
		R8G8B8A8_Unorm,
		
	};
	inline String TextureFormatToString(ETextureFormat format)
	{
		switch (format)
		{
		case ETextureFormat::R8G8B8A8_Unorm: return TT("R8G8B8A8_Unorm");
		default: return TT("None");
		}
	}

	enum class ShaderDataType
	{
		unknown,
		_bool, 
		_int, _int2, _int3, _int4,
		_uint, _uint2, _uint3, _uint4,
		_float, _float2, _float3, _float4, 
		_float3x3, _float4x4,
	};

	inline u64 GetShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::_bool: return 1;
		case ShaderDataType::_int:  return 4;
		case ShaderDataType::_int2: return 8;
		case ShaderDataType::_int3: return 12;
		case ShaderDataType::_int4: return 16;
		case ShaderDataType::_uint: return 4;
		case ShaderDataType::_uint2: return 8;
		case ShaderDataType::_uint3: return 12;
		case ShaderDataType::_uint4: return 16;
		case ShaderDataType::_float: return 4;
		case ShaderDataType::_float2: return 8;
		case ShaderDataType::_float3: return 12;
		case ShaderDataType::_float4: return 16;
		case ShaderDataType::_float3x3: return 36;
		case ShaderDataType::_float4x4: return 64;
		default:
			JG_CORE_CRITICAL("not supported ShaderDataType");
			return 0;
		}
	}
	inline String ShaderDataTypeToString(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::_bool:		return TT("bool");
		case ShaderDataType::_int:		return TT("int");
		case ShaderDataType::_int2:		return TT("int2");
		case ShaderDataType::_int3:		return TT("int3");
		case ShaderDataType::_int4:		return TT("int4");
		case ShaderDataType::_uint:		return TT("uint");
		case ShaderDataType::_uint2:	return TT("uint2");
		case ShaderDataType::_uint3:	return TT("uint3");
		case ShaderDataType::_uint4:	return TT("uin4");
		case ShaderDataType::_float:	return TT("float");
		case ShaderDataType::_float2:	return TT("float2");
		case ShaderDataType::_float3:	return TT("float3");
		case ShaderDataType::_float4:   return TT("float4");
		case ShaderDataType::_float3x3: return TT("float3x3");
		case ShaderDataType::_float4x4: return TT("float4x4");
		default:
			JG_CORE_CRITICAL("not supported ShaderDataType");
			return TT("unknown");
		}
	}





	class ScissorRect
	{
	public:
		i32    Left = 0;
		i32    Top  = 0;
		i32    Right= 0;
		i32    Bottom = 0;

	public:
		ScissorRect(i32 left, i32 top, i32 right, i32 bottom) :
			Left(left), Top(top), Right(right), Bottom(bottom){}
	};


	class Viewport
	{
	public:
		f32 TopLeftX = 0.0f;
		f32 TopLeftY = 0.0f;
		f32 Width    = 0.0f;
		f32 Height   = 0.0f;
		f32 MinDepth = 0.0f;
		f32 MaxDepth = 1.0f;
	public:
		Viewport(f32 w, f32 h) : Width(w), Height(h) {}
		Viewport(f32 topLeftX, f32 topLeftY, f32 w, f32 h) {}
	};






	/* InputLayout 
	
	
	*/

	
	
	



	namespace ShaderCode
	{
		namespace HLSL
		{
			constexpr wchar* Common = LR"(
struct SDObject
{
	float4x4 world;
};

cbuffer CBCamera : register(b0, space0)
{
	float4x4 gViewProj;
};


StructuredBuffer<SDObject>      gObjectInstances : register(t0, space0);
)";



			constexpr wchar* SpriteShader = LR"(
struct VS_IN
{
	float3 posL : POSITION;
	float3 tex  : TEXCOORD;
}

struct VS_OUT
{
	float4 posH : SV_POSITION;
	float2 tex   : TEXCOORD;
}

VS_OUT vs(VS_IN vin, uint insID)
{
	VS_OUT vout;
	float4x4 world = gObjectInstances[insID].world;
	float4 posW = mul(float4(vin.posL, 1.0f), world);


	vout.posH = mul(posW, g_viewProj);
	vout.tex = vin.tex;
	return vout;
}

float4 ps(VS_OUT pin) : SV_TARGET
{

	return float4(1.0f,1.0f,1.0f,1.0f);
}

)";

			constexpr wchar* TextShader = LR"()";
		}
	
	}
}