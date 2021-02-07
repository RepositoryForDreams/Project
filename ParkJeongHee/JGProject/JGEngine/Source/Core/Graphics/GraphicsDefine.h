#pragma once
#include "JGCore.h"






namespace JG
{
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

	
	
	
}