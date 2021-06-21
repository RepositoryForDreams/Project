#pragma once
#include "Define.h"
#include <Windows.h>
#include <string>

#define ENUM_FLAG(enumName) \
enum class enumName; \
inline enumName operator|(enumName c1, enumName c2) \
{ \
	return (enumName)((int)c1 | (int)c2); \
} \
inline bool operator&(enumName c1, enumName c2) \
{ \
	return (bool)((int)c1 & (int)c2); \
} \

namespace JG
{
	enum class EWindowPlatform
	{
		Windows,
	};
	enum class EGraphicsAPI
	{
		DirectX12,
		DirectX11,
		OpenGL,
		Vulkan,
	};
	inline String GraphicsAPIToString(EGraphicsAPI api)
	{
		switch(api)
		{
			case EGraphicsAPI::DirectX12: return TT("DirectX12");
			case EGraphicsAPI::DirectX11: return TT("DirectX11");
			case EGraphicsAPI::OpenGL:    return TT("OpenGL");
			case EGraphicsAPI::Vulkan:    return TT("Vulkan");
			default: return TT("None");
		}
	}
	enum class EAssetFormat
	{
		None,
		Mesh,
		Skeletal,
		Material,
		Texture,
		Directory,
		GameWorld,
	};

	enum class EKeyCode
	{
		Unknown = 0,
		Num0 = 0x30, Num1 = 0x31, Num2 = 0x32, Num3 = 0x33, Num4 = 0x34,
		Num5 = 0x35, Num6 = 0x36, Num7 = 0x37, Num8 = 0x38, Num9 = 0x39,
		// 숫자 패드 키코드
		NumPad0 = 0x60, NumPad1 = 0x61, NumPad2 = 0x62, NumPad3  = 0x63, NumPad4 = 0x64,
		NumPad5 = 0x65, NumPad6 = 0x66, NumPad7 = 0x67, NumPad8 = 0x68, NumPad9 = 0x69,
		// F 숫자 키코드
		F1  = 0x70, F2  = 0x71, F3  = 0x72, F4  = 0x73, F5  = 0x74, F6  = 0x75,
		F7  = 0x76, F8  = 0x77, F9  = 0x78, F10 = 0x79, F11 = 0x7A, F12 = 0x7B,
		// 영어 키코드
		A = 0x41, B = 0x42, C = 0x43, D = 0x44, E = 0x45,
		F = 0x46, G = 0x47, H = 0x48, I = 0x49, J = 0x4A,
		K = 0x4B, L = 0x4C, N = 0x4E, M = 0x4D, O = 0x4F,
		P = 0x50, Q = 0x51, R = 0x52, S = 0x53, T = 0x54,
	    U = 0x55, V = 0x56, W = 0x57, X = 0x58, Y = 0x59,
		Z = 0x5A,

		BackSpace		= VK_BACK,      Tab				= VK_TAB,
		Clear			= VK_CLEAR,     Enter			= VK_RETURN,
		Shift			= VK_SHIFT,		Ctrl			= VK_CONTROL,
		Alt				= VK_MENU,		Pause			= VK_PAUSE,
		CapsLock		= VK_CAPITAL,	Esc				= VK_ESCAPE,
		Space			= VK_SPACE,		PgUp			= VK_PRIOR,
		PgDown			= VK_NEXT,		End				= VK_END,
		Home			= VK_HOME,		Left			= VK_LEFT,
		Up				= VK_UP,		Right			= VK_RIGHT,
		Down            = VK_DOWN,		Select			= VK_SELECT,
		Print			= VK_PRINT,		Execute			= VK_EXECUTE,
		PrintScreen		= VK_SNAPSHOT,	Insert			= VK_INSERT,
		Help			= VK_HELP,		NumPadAdd		= VK_ADD,
		NumPadMultiply  = VK_MULTIPLY,	NumPadSubtract  = VK_SUBTRACT,
		NumPadDemical   = VK_DECIMAL,	NumPadDivide	= VK_DIVIDE,
		NumLock			= VK_NUMLOCK,	ScrollLock		= VK_SCROLL,
		LeftShift	    = VK_LSHIFT,	RightShift		= VK_RSHIFT,
		LeftCtrl		= VK_LCONTROL,	RightCtrl		= VK_RCONTROL,
		LeftAlt			= VK_LMENU,		RightAlt		= VK_RMENU,
		LeftBracket		= VK_OEM_4,		RightBracket	= VK_OEM_6,
		BackSlash		= VK_OEM_5,		QuotationMark	= VK_OEM_7,
		Semicolon		= VK_OEM_1,		Plus			= VK_OEM_PLUS,
		Comma			= VK_OEM_COMMA,	Minus			= VK_OEM_MINUS,
		Period			= VK_OEM_PERIOD,Slash			= VK_OEM_2,
		GraveAccent		= VK_OEM_3
	};
	inline String KeyCodeToString(EKeyCode code)
	{
		switch (code)
		{
		case EKeyCode::Num0:			return TT("0");
		case EKeyCode::Num1:			return TT("1");
		case EKeyCode::Num2:			return TT("2");
		case EKeyCode::Num3:			return TT("3");
		case EKeyCode::Num4:			return TT("4");
		case EKeyCode::Num5:			return TT("5");
		case EKeyCode::Num6:			return TT("6");
		case EKeyCode::Num7:			return TT("7");
		case EKeyCode::Num8:			return TT("8");
		case EKeyCode::Num9:			return TT("9");
		case EKeyCode::NumPad0:			return TT("NumPad0");
		case EKeyCode::NumPad1:			return TT("NumPad1");
		case EKeyCode::NumPad2:			return TT("NumPad2");
		case EKeyCode::NumPad3:			return TT("NumPad3");
		case EKeyCode::NumPad4:			return TT("NumPad4");
		case EKeyCode::NumPad5:			return TT("NumPad5");
		case EKeyCode::NumPad6:			return TT("NumPad6");
		case EKeyCode::NumPad7:			return TT("NumPad7");
		case EKeyCode::NumPad8:			return TT("NumPad8");
		case EKeyCode::NumPad9:			return TT("NumPad9");
		case EKeyCode::F1:				return TT("F1");
		case EKeyCode::F2:				return TT("F2");
		case EKeyCode::F3:				return TT("F3");
		case EKeyCode::F4:				return TT("F4");
		case EKeyCode::F5:				return TT("F5");
		case EKeyCode::F6:				return TT("F6");
		case EKeyCode::F7:				return TT("F7");
		case EKeyCode::F8:				return TT("F8");
		case EKeyCode::F9:				return TT("F9");
		case EKeyCode::F10:				return TT("F10");
		case EKeyCode::F11:				return TT("F11");
		case EKeyCode::F12:				return TT("F12");
		case EKeyCode::A:				return TT("A");
		case EKeyCode::B:				return TT("B");
		case EKeyCode::C:				return TT("C");
		case EKeyCode::D:				return TT("D");
		case EKeyCode::E:				return TT("E");
		case EKeyCode::F:				return TT("F");
		case EKeyCode::G:				return TT("G");
		case EKeyCode::H:				return TT("H");
		case EKeyCode::I:				return TT("I");
		case EKeyCode::J:				return TT("J");
		case EKeyCode::K:				return TT("K");
		case EKeyCode::L:				return TT("L");
		case EKeyCode::N:				return TT("N");
		case EKeyCode::M:				return TT("M");
		case EKeyCode::O:				return TT("O");
		case EKeyCode::P:				return TT("P");
		case EKeyCode::Q:				return TT("Q");
		case EKeyCode::R:				return TT("R");
		case EKeyCode::S:				return TT("S");
		case EKeyCode::T:				return TT("T");
		case EKeyCode::U:				return TT("U");
		case EKeyCode::V:				return TT("V");
		case EKeyCode::W:				return TT("W");
		case EKeyCode::X:				return TT("X");
		case EKeyCode::Y:				return TT("Y");
		case EKeyCode::Z:				return TT("Z");
		case EKeyCode::BackSpace:		return TT("BackSpace");
		case EKeyCode::Tab:				return TT("Tab");
		case EKeyCode::Clear:			return TT("Clear");
		case EKeyCode::Enter:			return TT("Enter");
		case EKeyCode::Shift:			return TT("Shift");
		case EKeyCode::Ctrl:			return TT("Ctrl");
		case EKeyCode::Alt:				return TT("Alt");
		case EKeyCode::Pause:			return TT("Pause");
		case EKeyCode::CapsLock:		return TT("CapsLock");
		case EKeyCode::Esc:				return TT("Esc");
		case EKeyCode::Space:			return TT("Space");
		case EKeyCode::PgUp:			return TT("PgUp");
		case EKeyCode::PgDown:			return TT("PgDown");
		case EKeyCode::End:				return TT("End");
		case EKeyCode::Home:			return TT("Home");
		case EKeyCode::Left:			return TT("Left");
		case EKeyCode::Up:				return TT("Up");
		case EKeyCode::Right:			return TT("Right");
		case EKeyCode::Down:			return TT("Down");
		case EKeyCode::Select:			return TT("Select");
		case EKeyCode::Print:			return TT("Print");
		case EKeyCode::Execute:			return TT("Execute");
		case EKeyCode::PrintScreen:		return TT("PrintScreen");
		case EKeyCode::Insert:			return TT("Insert");
		case EKeyCode::Help:			return TT("Help");
		case EKeyCode::NumPadAdd:		return TT("NumPadAdd");
		case EKeyCode::NumPadMultiply:	return TT("NumPadMultiply");
		case EKeyCode::NumPadSubtract:	return TT("NumPadSubtract");
		case EKeyCode::NumPadDemical:	return TT("NumPadDecimal");
		case EKeyCode::NumPadDivide:	return TT("NumPadDivide");
		case EKeyCode::NumLock:			return TT("NumLock");
		case EKeyCode::ScrollLock:		return TT("ScrollLock");
		case EKeyCode::LeftShift:		return TT("LeftShift");
		case EKeyCode::RightShift:		return TT("RightShift");
		case EKeyCode::LeftCtrl:		return TT("LeftCtrl");
		case EKeyCode::RightCtrl:		return TT("RightCtrl");
		case EKeyCode::LeftAlt:			return TT("LeftAlt");
		case EKeyCode::RightAlt:		return TT("RightAlt");
		case EKeyCode::LeftBracket:		return TT("[");
		case EKeyCode::RightBracket:	return TT("]");
		case EKeyCode::BackSlash:		return TT("'\'");
		case EKeyCode::QuotationMark:	return TT("'");
		case EKeyCode::Semicolon:		return TT(";");
		case EKeyCode::Comma:			return TT(",");
		case EKeyCode::Plus:			return TT("+");
		case EKeyCode::Minus:			return TT("-");
		case EKeyCode::Period:			return TT(".");
		case EKeyCode::Slash:			return TT("/");
		case EKeyCode::GraveAccent:		return TT("`");
		default:
			return TT("None");
		}
	}


	inline EKeyCode StringToKeyCode(const String& code)
	{
		
		if (code == TT("0")) { return  EKeyCode::Num0; }
		else if(code == TT("1") ) { return  EKeyCode::Num1;	}		
		else if(code == TT("2") ) { return  EKeyCode::Num2;	}		
		else if(code == TT("3") ) { return  EKeyCode::Num3;	}		
		else if(code == TT("4") ) { return  EKeyCode::Num4;	}		
		else if(code == TT("5") ) { return  EKeyCode::Num5;	}		
		else if(code == TT("6") ) { return  EKeyCode::Num6;	}		
		else if(code == TT("7") ) { return  EKeyCode::Num7;	}		
		else if(code == TT("8") ) { return  EKeyCode::Num8;	}		
		else if(code == TT("9") ) { return  EKeyCode::Num9;	}		
		else if(code == TT("NumPad0")) {return  EKeyCode::NumPad0; }		
		else if(code == TT("NumPad1")) {return  EKeyCode::NumPad1; }		
		else if(code == TT("NumPad2")) {return  EKeyCode::NumPad2; }		
		else if(code == TT("NumPad3")) {return  EKeyCode::NumPad3; }		
		else if(code == TT("NumPad4")) {return  EKeyCode::NumPad4; }		
		else if(code == TT("NumPad5")) {return  EKeyCode::NumPad5; }		
		else if(code == TT("NumPad6")) {return  EKeyCode::NumPad6; }		
		else if(code == TT("NumPad7")) {return  EKeyCode::NumPad7; }		
		else if(code == TT("NumPad8")) {return  EKeyCode::NumPad8; }		
		else if(code == TT("NumPad9")) {return  EKeyCode::NumPad9; }		
		else if(code == TT("F1") ) { return EKeyCode::F1; }				
		else if(code == TT("F2") ) { return EKeyCode::F2; }				
		else if(code == TT("F3") ) { return EKeyCode::F3; }				
		else if(code == TT("F4") ) { return EKeyCode::F4; }				
		else if(code == TT("F5") ) { return EKeyCode::F5; }				
		else if(code == TT("F6") ) { return EKeyCode::F6; }				
		else if(code == TT("F7") ) { return EKeyCode::F7; }				
		else if(code == TT("F8") ) { return EKeyCode::F8; }				
		else if(code == TT("F9") ) { return EKeyCode::F9; }				
		else if(code == TT("F10")) { return EKeyCode::F10;}				
		else if(code == TT("F11")) { return EKeyCode::F11;}				
		else if(code == TT("F12")) { return EKeyCode::F12;}				
		else if(code == TT("A")) { return EKeyCode::A; }				
		else if(code == TT("B")) { return EKeyCode::B; }				
		else if(code == TT("C")) { return EKeyCode::C; }				
		else if(code == TT("D")) { return EKeyCode::D; }				
		else if(code == TT("E")) { return EKeyCode::E; }				
		else if(code == TT("F")) { return EKeyCode::F; }				
		else if(code == TT("G")) { return EKeyCode::G; }				
		else if(code == TT("H")) { return EKeyCode::H; }				
		else if(code == TT("I")) { return EKeyCode::I; }				
		else if(code == TT("J")) { return EKeyCode::J; }				
		else if(code == TT("K")) { return EKeyCode::K; }				
		else if(code == TT("L")) { return EKeyCode::L; }				
		else if(code == TT("N")) { return EKeyCode::N; }				
		else if(code == TT("M")) { return EKeyCode::M; }				
		else if(code == TT("O")) { return EKeyCode::O; }				
		else if(code == TT("P")) { return EKeyCode::P; }				
		else if(code == TT("Q")) { return EKeyCode::Q; }				
		else if(code == TT("R")) { return EKeyCode::R; }				
		else if(code == TT("S")) { return EKeyCode::S; }				
		else if(code == TT("T")) { return EKeyCode::T; }				
		else if(code == TT("U")) { return EKeyCode::U; }				
		else if(code == TT("V")) { return EKeyCode::V; }				
		else if(code == TT("W")) { return EKeyCode::W; }				
		else if(code == TT("X")) { return EKeyCode::X; }				
		else if(code == TT("Y")) { return EKeyCode::Y; }				
		else if(code == TT("Z")) { return EKeyCode::Z; }				
		else if (code == TT("BackSpace"))	{ return EKeyCode::BackSpace; }
		else if (code == TT("Tab"))			{ return EKeyCode::Tab; }
		else if (code == TT("Clear"))		{ return EKeyCode::Clear; }
		else if (code == TT("Enter"))		{ return EKeyCode::Enter; }
		else if (code == TT("Shift"))		{ return EKeyCode::Shift; }
		else if (code == TT("Ctrl"))		{ return EKeyCode::Ctrl; }
		else if (code == TT("Alt"))			{ return EKeyCode::Alt; }
		else if (code == TT("Pause"))		{ return EKeyCode::Pause; }
		else if (code == TT("CapsLock"))	{ return EKeyCode::CapsLock; }
		else if (code == TT("Esc"))			{ return EKeyCode::Esc; }
		else if (code == TT("Space"))		{ return EKeyCode::Space; }
		else if (code == TT("PgUp"))		{ return EKeyCode::PgUp; }
		else if (code == TT("PgDown"))		{ return EKeyCode::PgDown; }
		else if (code == TT("End"))			{ return EKeyCode::End; }
		else if (code == TT("Home"))		{ return EKeyCode::Home; }
		else if (code == TT("Left"))		{ return EKeyCode::Left; }
		else if (code == TT("Up"))			{ return EKeyCode::Up; }
		else if (code == TT("Right"))		{ return EKeyCode::Right; }
		else if (code == TT("Down"))		{ return EKeyCode::Down; }
		else if (code == TT("Select"))		{ return EKeyCode::Select; }
		else if (code == TT("Print"))		{ return EKeyCode::Print; }
		else if (code == TT("Execute"))		{ return EKeyCode::Execute; }
		else if (code == TT("PrintScreen")) { return EKeyCode::PrintScreen; }
		else if (code == TT("Insert")) { return EKeyCode::Insert; }
		else if (code == TT("Help")) { return EKeyCode::Help; }
		else if (code == TT("NumPadAdd")) { return EKeyCode::NumPadAdd; }
		else if (code == TT("NumPadMultiply")) { return EKeyCode::NumPadMultiply; }
		else if (code == TT("NumPadSubtract")) { return EKeyCode::NumPadSubtract; }
		else if (code == TT("NumPadDecimal")) { return EKeyCode::NumPadDemical; }
		else if (code == TT("NumPadDivide")) { return EKeyCode::NumPadDivide; }
		else if (code == TT("NumLock")) { return EKeyCode::NumLock; }
		else if (code == TT("ScrollLock")) { return EKeyCode::ScrollLock; }
		else if (code == TT("LeftShift")) { return EKeyCode::LeftShift; }
		else if (code == TT("RightShift")) { return EKeyCode::RightShift; }
		else if (code == TT("LeftCtrl")) { return EKeyCode::LeftCtrl; }
		else if (code == TT("RightCtrl")) { return EKeyCode::RightCtrl; }
		else if (code == TT("LeftAlt")) { return EKeyCode::LeftAlt; }
		else if (code == TT("RightAlt")) { return EKeyCode::RightAlt; }
		else if (code == TT("[")) { return EKeyCode::LeftBracket; }
		else if (code == TT("]")) { return EKeyCode::RightBracket; }
		else if (code == TT("'\'")) { return EKeyCode::BackSlash; }
		else if (code == TT("'")) { return EKeyCode::QuotationMark; }
		else if (code == TT(";")) { return EKeyCode::Semicolon; }
		else if (code == TT(",")) { return EKeyCode::Comma; }
		else if (code == TT("+")) { return EKeyCode::Plus; }
		else if (code == TT("-")) { return EKeyCode::Minus; }
		else if (code == TT(".")) { return EKeyCode::Period; }
		else if (code == TT("/")) { return EKeyCode::Slash; }
		else if (code == TT("`")) { return EKeyCode::GraveAccent; }
		else { return EKeyCode::Unknown; }
	}


	enum class EInputAction
	{
		Pressed,
		Released,
	};
	inline String InputActionToString(EInputAction inputAction)
	{
		switch(inputAction)
		{
		case EInputAction::Pressed:  return TT("Pressed");
		case EInputAction::Released: return TT("Released");
		default:
			return TT("None");
		}
	}

	

	enum class EInputMode
	{
		Default,
	};

	enum class EMouseButton
	{
		Left,
		Right,
		Middle,
		XButton1,
		XButton2,
	};
	inline String MouseButtonToString(EMouseButton inputAction)
	{
		switch (inputAction)
		{
		case EMouseButton::Left:     return TT("LeftButton");
		case EMouseButton::Right:    return TT("RightButton");
		case EMouseButton::Middle:   return TT("MiddleButton");
		case EMouseButton::XButton1: return TT("XButton1");
		case EMouseButton::XButton2: return TT("XButton2");
		default:
			return TT("None");
		}
	}
}