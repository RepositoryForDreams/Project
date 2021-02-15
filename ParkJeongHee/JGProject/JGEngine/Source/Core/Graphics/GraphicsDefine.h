#pragma once
#include "JGCore.h"






namespace JG
{
#define MAX_RENDERTARGET 8

	using TextureID = u64;
	
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


	ENUM_FLAG(ETextureFlags)
		enum class ETextureFlags
	{
		None			   = 0x00001,
		Allow_RenderTarget = 0x00002,
		Allow_DepthStencil = 0x00004,
		
	};

	struct TextureInfo
	{
		u32 Width = 0;
		u32 Height = 0;
		ETextureFormat Format = ETextureFormat::None;
		ETextureFlags Flags   = ETextureFlags::None;
	};



	ENUM_FLAG(EShaderFlags)
		enum class EShaderFlags
	{
		None = 0x000,
		Allow_VertexShader = 0x001,
		Allow_PixelShader = 0x002,
		Allow_GeometryShader = 0x004,
		Allow_HullShader = 0x008,
		Allow_DomainShader = 0x016,


	};

	
	enum class EShaderDataType
	{
		unknown,
		_bool, 
		_int, _int2, _int3, _int4,
		_uint, _uint2, _uint3, _uint4,
		_float, _float2, _float3, _float4, 
		_float3x3, _float4x4,
	};

	inline u64 GetShaderDataTypeSize(EShaderDataType type)
	{
		switch (type)
		{
		case EShaderDataType::_bool: return 1;
		case EShaderDataType::_int:  return 4;
		case EShaderDataType::_int2: return 8;
		case EShaderDataType::_int3: return 12;
		case EShaderDataType::_int4: return 16;
		case EShaderDataType::_uint: return 4;
		case EShaderDataType::_uint2: return 8;
		case EShaderDataType::_uint3: return 12;
		case EShaderDataType::_uint4: return 16;
		case EShaderDataType::_float: return 4;
		case EShaderDataType::_float2: return 8;
		case EShaderDataType::_float3: return 12;
		case EShaderDataType::_float4: return 16;
		case EShaderDataType::_float3x3: return 36;
		case EShaderDataType::_float4x4: return 64;
		default:
			JG_CORE_CRITICAL("not supported ShaderDataType");
			return 0;
		}
	}
	inline String ShaderDataTypeToString(EShaderDataType type)
	{
		switch (type)
		{
		case EShaderDataType::_bool:		return TT("bool");
		case EShaderDataType::_int:		return TT("int");
		case EShaderDataType::_int2:		return TT("int2");
		case EShaderDataType::_int3:		return TT("int3");
		case EShaderDataType::_int4:		return TT("int4");
		case EShaderDataType::_uint:		return TT("uint");
		case EShaderDataType::_uint2:	return TT("uint2");
		case EShaderDataType::_uint3:	return TT("uint3");
		case EShaderDataType::_uint4:	return TT("uin4");
		case EShaderDataType::_float:	return TT("float");
		case EShaderDataType::_float2:	return TT("float2");
		case EShaderDataType::_float3:	return TT("float3");
		case EShaderDataType::_float4:   return TT("float4");
		case EShaderDataType::_float3x3: return TT("float3x3");
		case EShaderDataType::_float4x4: return TT("float4x4");
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