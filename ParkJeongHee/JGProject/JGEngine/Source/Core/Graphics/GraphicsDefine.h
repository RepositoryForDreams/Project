#pragma once
#include "JGCore.h"
#include <dxgi1_6.h>





namespace JG
{
#define MAX_RENDERTARGET 8
#define TEXTURE_ID_NULL  0
	using TextureID = u64;

	enum class ETextureFormat
	{
		None,
		R8G8B8A8_Unorm,
		R16G16B16A16_Unorm,
		D24_Unorm_S8_Uint
	};
	inline String TextureFormatToString(ETextureFormat format)
	{
		switch (format)
		{
		case ETextureFormat::R8G8B8A8_Unorm: return TT("R8G8B8A8_Unorm");
		case ETextureFormat::R16G16B16A16_Unorm: return TT("R16G16B16A16_Unorm");
		case ETextureFormat::D24_Unorm_S8_Uint: return TT("D24_Unorm_S8_Uint");
		default: return TT("None");
		}
	}


	inline DXGI_FORMAT ConvertDXGIFormat(ETextureFormat format)
	{

		switch (format)
		{
		case ETextureFormat::R8G8B8A8_Unorm:     return DXGI_FORMAT_R8G8B8A8_UNORM;
		case ETextureFormat::R16G16B16A16_Unorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
		case ETextureFormat::D24_Unorm_S8_Uint:  return	DXGI_FORMAT_D24_UNORM_S8_UINT;
		default:
			JG_CORE_ERROR("This {0} DirectX12 TextureFormat is not supported convert ETextureFormat", TextureFormatToString(format));
			return DXGI_FORMAT_UNKNOWN;
		}
	}






	ENUM_FLAG(ETextureFlags)
		enum class ETextureFlags
	{
		None = 0x00001,
		Allow_RenderTarget = 0x00002,
		Allow_DepthStencil = 0x00004,
	};




	struct TextureInfo
	{
		u32 Width = 0;
		u32 Height = 0;
		ETextureFormat Format = ETextureFormat::None;
		ETextureFlags  Flags = ETextureFlags::None;
		u16 MipLevel = 0;
		u16 ArraySize = 1;


		Color ClearColor;
		f32   ClearDepth = 1.0f;
		u8    ClearStencil = 0;
	};



	ENUM_FLAG(EShaderFlags)
		enum class EShaderFlags
	{
		None = 0x000,
		Allow_VertexShader   = 0x001,
		Allow_PixelShader    = 0x002,
		Allow_GeometryShader = 0x004,
		Allow_HullShader     = 0x008,
		Allow_DomainShader   = 0x010,
		Allow_ComputeShader = 0x020,
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
		case EShaderDataType::_bool:	return TT("bool");
		case EShaderDataType::_int:		return TT("int");
		case EShaderDataType::_int2:	return TT("int2");
		case EShaderDataType::_int3:	return TT("int3");
		case EShaderDataType::_int4:	return TT("int4");
		case EShaderDataType::_uint:	return TT("uint");
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
	inline EShaderDataType  StringToShaderDataType(const String& type)
	{
		if (type == TT("bool")) return EShaderDataType::_bool;
		else if (type == TT("int")) return EShaderDataType::_int;
		else if (type == TT("int2")) return EShaderDataType::_int2;
		else if (type == TT("int3")) return EShaderDataType::_int3;
		else if (type == TT("int4")) return EShaderDataType::_int4;
		else if (type == TT("uint")) return EShaderDataType::_uint;
		else if (type == TT("uint2")) return EShaderDataType::_uint2;
		else if (type == TT("uint3")) return EShaderDataType::_uint3;
		else if (type == TT("uint4")) return EShaderDataType::_uint4;
		else if (type == TT("float")) return EShaderDataType::_float;
		else if (type == TT("float2")) return EShaderDataType::_float2;
		else if (type == TT("float3")) return EShaderDataType::_float3;
		else if (type == TT("float4")) return EShaderDataType::_float4;
		else if (type == TT("float3x3")) return EShaderDataType::_float3x3;
		else if (type == TT("float4x4")) return EShaderDataType::_float4x4;
		else
		{
			return EShaderDataType::unknown;
		}
	}

	enum class ESamplerStateTemplate
	{
		Unknown,
		Point_Wrap,
		Point_Clamp,
		Point_Border,
		Point_Mirror,
		Point_MirrorOnce,
		Linear_Wrap,
		Linear_Clamp,
		Linear_Border,
		Linear_Mirror,
		Linear_MirrorOnce,
		Anisotropic_Wrap,
		Anisotropic_Clamp,
		Anisotropic_Border,
		Anisotropic_Mirror,
		Anisotropic_MirrorOnce,
	};

	inline ESamplerStateTemplate StringToSamplerStateTemplate(const String& _template)
	{
		if (_template == TT("Point_Wrap")) return ESamplerStateTemplate::Point_Wrap;
		else if (_template == TT("Point_Clamp")) return ESamplerStateTemplate::Point_Clamp;
		else if (_template == TT("Point_Border")) return ESamplerStateTemplate::Point_Border;
		else if (_template == TT("Point_Mirror")) return ESamplerStateTemplate::Point_Mirror;
		else if (_template == TT("Point_MirrorOnce")) return ESamplerStateTemplate::Point_MirrorOnce;
		else if (_template == TT("Linear_Wrap")) return ESamplerStateTemplate::Linear_Wrap;
		else if (_template == TT("Linear_Clamp")) return ESamplerStateTemplate::Linear_Clamp;
		else if (_template == TT("Linear_Border")) return ESamplerStateTemplate::Linear_Border;
		else if (_template == TT("Linear_Mirror")) return ESamplerStateTemplate::Linear_Mirror;
		else if (_template == TT("Linear_MirrorOnce")) return ESamplerStateTemplate::Linear_MirrorOnce;
		else if (_template == TT("Anisotropic_Wrap")) return ESamplerStateTemplate::Anisotropic_Wrap;
		else if (_template == TT("Anisotropic_Clamp")) return ESamplerStateTemplate::Anisotropic_Clamp;
		else if (_template == TT("Anisotropic_Border")) return ESamplerStateTemplate::Anisotropic_Border;
		else if (_template == TT("Anisotropic_Mirror")) return ESamplerStateTemplate::Anisotropic_Mirror;
		else if (_template == TT("Anisotropic_MirrorOnce")) return ESamplerStateTemplate::Anisotropic_MirrorOnce;
		else return ESamplerStateTemplate::Unknown;
	}


	enum class EDepthStencilStateTemplate
	{
		Default = 0,
		NoDepth = 1,
	};

	enum class EBlendStateTemplate
	{
		Default = 0,
		Transparent_Default,
	};
	//D3D12_RASTERIZER_DESC
	enum class ERasterizerStateTemplate
	{
		Default = 0,
	};

	class DepthStencilState
	{

	};
	class BlendState
	{

	};
	class RasterizerState
	{

	};


	enum class EBufferLoadMethod
	{
		GPULoad,
		CPULoad,
	};
	enum class EComputeBufferState
	{
		Wait,
		Run,
		Compelete,
	};
	using EComputerState = EComputeBufferState;
	







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



	// RenderItem
	// ·»´õ¸µ Á¤º¸
	class IMesh;
	class IMaterial;
	class ITexture;
	class IRenderItem : public IJGObject
	{
		JGCLASS
	public:
		String TargetLayer  = TT("Default");
		JMatrix WorldMatrix = JMatrix::Identity();
		SharedPtr<IMesh>	 Mesh = nullptr;
		SharedPtr<IMaterial> Material = nullptr;
	public:
		virtual ~IRenderItem() = default;
	};

	class StandardSpriteRenderItem : public IRenderItem
	{
		JGCLASS
	public:
		Color Color = Color::White();
		String SortingLayer;
		i64    SortingOrder = 0;
	public:
		virtual ~StandardSpriteRenderItem() = default;
	};

	class CustomSpriteRenderItem : public IRenderItem
	{
		JGCLASS
	public:
		virtual ~CustomSpriteRenderItem() = default;
	};

	class MeshRenderItem : public IRenderItem
	{
		JGCLASS
	public:
		virtual ~MeshRenderItem() = default;
	};


	namespace ShaderScript
	{
		constexpr wchar* Standard2DShader = TT("Standard2DShader");
	}


	namespace HLSL
	{
		

		using token = wchar;
		namespace Token
		{
			constexpr token* Struct                 = TT("struct ");
			constexpr token* CBuffer                = TT("cbuffer ");
			constexpr token* StructuredBuffer       = TT("StructuredBuffer");
			constexpr token* Texture2D              = TT("Texture2D ");
			constexpr token* RWStructuredBuffer		= TT("RWStructuredBuffer");
			constexpr token* RWTexture2D			= TT("RWTexture2D ");
			constexpr token* SamplerState           = TT("SamplerState ");
			constexpr token* SamplerComparisonState = TT("SamplerComparisonState ");
			


			namespace SamplerStateElement
			{
				constexpr token* Template = TT("Template");
				constexpr token* Min = TT("Min");
				constexpr token* Mag = TT("Mag");
				constexpr token* Mip = TT("Mip");
				constexpr token* AddressU = TT("AddressU");
				constexpr token* AddressV = TT("AddressV");
				constexpr token* AddressW = TT("AddressW");
				constexpr token* ComparisonFunc = TT("ComparisonFunc");
				constexpr token* BorderColor = TT("BorderColor");
				constexpr token* MinLOD = TT("MinLOD");
				constexpr token* MaxLOD = TT("MaxLOD");
				constexpr token* MaxAnisotropy = TT("MaxAnisotropy");
				constexpr token* MipLODBias = TT("MipLODBias");

			}
			namespace SamplerSatateFilter
			{
				constexpr token* Point = TT("Point");
				constexpr token* Linear = TT("Linear");
				constexpr token* Anisotropic = TT("Anisotropic");

			}
			namespace SamplerSatateAddressMode
			{
				constexpr token* Wrap = TT("Wrap");
				constexpr token* Mirror = TT("Mirror");
				constexpr token* Clamp = TT("Clamp");
				constexpr token* Border = TT("Border");
				constexpr token* MirrorOnce = TT("MirrorOnce");
			}
			namespace SamplerStateComparisonFunc
			{
				constexpr token* Never = TT("Never");
				constexpr token* Less = TT("Less");
				constexpr token* Equal = TT("Equal");
				constexpr token* LessEqual = TT("LessEqual");
				constexpr token* Greater = TT("Greater");
				constexpr token* NotEqual = TT("NotEqual");
				constexpr token* GreaterEqual = TT("GreaterEqual");
				constexpr token* Always = TT("Always");
			}
			namespace SamplerStateBorderColor
			{
				constexpr token* TransparentBlack = TT("TransparentBlack");
				constexpr token* OpaqueBlack = TT("OpaqueBlack");
				constexpr token* OpaqueWhite = TT("OpaqueWhite");
			}


		}


		constexpr wchar* VSEntry = TT("vs_main");
		constexpr wchar* DSEntry = TT("ds_main");
		constexpr wchar* HSEntry = TT("hs_main");
		constexpr wchar* GSEntry = TT("gs_main");
		constexpr wchar* PSEntry = TT("ps_main");
		constexpr wchar* CSEntry = TT("main");
		constexpr wchar* VSTarget = TT("vs_5_1");
		constexpr wchar* DSTarget = TT("ds_5_1");
		constexpr wchar* HSTarget = TT("hs_5_1");
		constexpr wchar* GSTarget = TT("gs_5_1");
		constexpr wchar* PSTarget = TT("ps_5_1");
		constexpr wchar* CSTarget = TT("cs_5_1");




		enum class EHLSLElement
		{
			None,
			CBuffer,
			StructuredBuffer,
			RWStructuredBuffer,
			Texture,
			RWTexture,
			SamplerState,
		};

		enum class EHLSLTextureType
		{
			_1D,
			_2D,
			_3D,
			Cube,
		};
		inline String GetHLSLElementToken(EHLSLElement)
		{
			return TT("");
		}

		inline String ShaderDataTypeToHLSLCode(EShaderDataType type)
		{
			switch (type)
			{
			case EShaderDataType::_bool:	return TT("bool");
			case EShaderDataType::_int:		return TT("int");
			case EShaderDataType::_int2:	return TT("int2");
			case EShaderDataType::_int3:	return TT("int3");
			case EShaderDataType::_int4:	return TT("int4");
			case EShaderDataType::_uint:	return TT("uint");
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



	}
}