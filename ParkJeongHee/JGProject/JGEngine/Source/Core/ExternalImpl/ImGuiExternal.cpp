#include "pch.h"
#include "ImGuiExternal.h"


namespace ImGui
{
	void OnGui(const char* label, JG::JVector3* v, float label_spacing)
	{
		std::string id = "##" + std::string(label) + "JVector3";


		ImGui::Spacing();
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label); ImGui::SameLine(); ImGui::Dummy(ImVec2(label_spacing, 0.0f)); ImGui::SameLine();
		ImGui::PushItemWidth(100.0f);
		ImGui::Text("X"); ImGui::SameLine(); auto id_x = id + "_X";  ImGui::InputFloat(id_x.c_str(), (float*)&(v->x)); ImGui::SameLine();
		ImGui::Text("Y"); ImGui::SameLine(); auto id_y = id + "_Y";  ImGui::InputFloat(id_y.c_str(), (float*)&(v->y)); ImGui::SameLine();
		ImGui::Text("Z"); ImGui::SameLine(); auto id_z = id + "_Z";  ImGui::InputFloat(id_z.c_str(), (float*)&(v->z));
		ImGui::PopItemWidth();
		ImGui::Spacing();
	}
	void OnGui(const char* label, JG::JVector2* v, float label_spacing)
	{
		std::string id = "##" + std::string(label) + "JVector2";

		ImGui::Spacing();
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label); ImGui::SameLine(); ImGui::Dummy(ImVec2(label_spacing, 0.0f)); ImGui::SameLine();
		ImGui::PushItemWidth(100.0f);
		ImGui::Text("X"); ImGui::SameLine(); auto id_x = id + "_X"; ImGui::InputFloat(id_x.c_str(), (float*)&(v->x)); ImGui::SameLine();
		ImGui::Text("Y"); ImGui::SameLine(); auto id_y = id + "_Y"; ImGui::InputFloat(id_y.c_str(), (float*)&(v->y));
		ImGui::PopItemWidth();
		ImGui::Spacing();
	}

	void OnGui(const char* label, JG::f32* v, float label_spacing )
	{
		std::string id = "##" + std::string(label) + "Float";

		ImGui::Spacing();
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label); ImGui::SameLine(); ImGui::Dummy(ImVec2(label_spacing, 0.0f)); ImGui::SameLine();
		ImGui::PushItemWidth(100.0f);
		ImGui::SameLine(); auto id_x = id + "_Value"; ImGui::InputFloat(id_x.c_str(), v); ImGui::SameLine();
		ImGui::PopItemWidth();
		ImGui::Spacing();
	}
	void OnGui(const char* label, JG::i64* v, float label_spacing)
	{
		std::string id = "##" + std::string(label) + "Int64";

		ImGui::Spacing();
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label); ImGui::SameLine(); ImGui::Dummy(ImVec2(label_spacing, 0.0f)); ImGui::SameLine();
		ImGui::PushItemWidth(100.0f);
		ImGui::SameLine(); auto id_x = id + "_Value"; ImGui::InputInt(id_x.c_str(), (int*)v); ImGui::SameLine();
		ImGui::PopItemWidth();
		ImGui::Spacing();
	}
}