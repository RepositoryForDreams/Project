#pragma once
#include "Define.h"





namespace JG
{
	class Color
	{
	public:
		f32 R = 0.0f;
		f32 G = 0.0f;
		f32 B = 0.0f;
		f32 A = 0.0F;
	public:
		Color() = default;
		Color(f32 r, f32 g, f32 b, f32 a) : R(r), G(g), B(b), A(a) {}

	public:
		static Color White() { return Color(1.0f, 1.0f, 1.0f, 1.0f); }
		static Color Black() { return Color(0.0f,0.0f,0.0f,1.0f); }
		static Color Red()   { return Color(1.0f, 0.0f, 0.0f, 1.0f); }
		static Color Blue()  { return Color(0.0f, 0.0f, 1.0f, 1.0f); }
		static Color Green() { return Color(0.0f, 1.0f, 0.0f, 1.0f); }
	};
}