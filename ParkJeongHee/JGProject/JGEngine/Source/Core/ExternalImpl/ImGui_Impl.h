#pragma once
#include "JGCore.h"
#include "Imgui/imgui.h"





struct ImVec4;
namespace ImGui
{

	void ExternalImpl_Init(JG::EWindowPlatform platform, JG::EGraphicsAPI API, JG::ptraddr handle);

	void ExternalImpl_Resize(JG::u32 width, JG::u32 height);
	void ExternalImpl_BeginFrame();
	void ExternalImpl_EndFrame(ImVec4 clearColor);
	void ExternalImpl_Destroy();
}