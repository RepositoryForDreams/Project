#pragma once

#include "JGCore.h"
#include "Imgui/imgui.h"


namespace ImGui
{
	void OnGui(const char* label, JG::JVector3* v, float label_spacing = 0.0f);
	void OnGui(const char* label, JG::JVector2* v, float label_spacing = 0.0f);
}