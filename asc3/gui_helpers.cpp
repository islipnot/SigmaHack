#include "pch.h"
#include "input.hpp"
#include "gui_helpers.hpp"

bool CheckboxEx(const char* const label, bool* const var)
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   { 4.0f, 6.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,  { 3.0f, 3.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,   1.5f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

	const bool v_status = *var;

	if (v_status)
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg,        GetColor(dark_purple));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetColor(light_purple));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  GetColor(light_purple));
	}
	else
	{
		constexpr ImVec4 light_grey = { 0.1562745f, 0.1562745f, 0.1562745f, 1.0f };
		ImGui::PushStyleColor(ImGuiCol_Border, light_grey);
	}

	const bool status = ImGui::Checkbox(label, var);

	ImGui::PopStyleColor(v_status ? 3 : 1);
	ImGui::PopStyleVar(4);

	return status;
}

int SliderFloatEx(const char* label, float* var, const float min, const float max)
{
	static auto style = ImGui::GetStyle();

	const ImVec2 FramePadding = { style.FramePadding.x, style.FramePadding.y * 0.60f };
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, FramePadding);
	ImGui::SetNextItemWidth(250.0f);

	const int status = ImGui::SliderFloat(label, var, min, max);
	ImGui::PopStyleVar();

	return status;
}

bool ComboEx(const char* label, int* selection, const char* items[], int item_count)
{
	ImGui::SameLine();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 5.0f, 4.0f });
	ImGui::SetNextItemWidth(100.0f);

	const bool status = ImGui::Combo(label, selection, items, item_count);

	ImGui::PopStyleVar();

	return status;
}

bool ResetButton(const int item_id)
{
	ImGui::SameLine();

	static const float padding_x = (ImGui::CalcTextSize("reset").x / 2) * 0.80f;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { padding_x, 3.0f });
	ImGui::PushID(item_id);

	const bool status = ImGui::Button("reset");
	ImGui::PopStyleVar();
	ImGui::PopID();

	return status;
}

void ToolTip(const char* const text)
{
	ImGui::SameLine();

	ImGui::TextColored(GetColor(light_purpleA), "?");

	if (ImGui::IsItemHovered()) ImGui::SetTooltip(text);
}

bool ColorEditEx(const char* label, float col[4])
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   { 4.0f, 6.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,  { 3.0f, 3.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,   1.5f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

	const bool status = ImGui::ColorEdit4(label, col, ImGuiColorEditFlags_NoInputs);

	ImGui::PopStyleVar(4);
	return status;
}

ImVec4 GetColor(const int original_color)
{
	const auto& accent = cfg.menu_accent;

	switch (original_color)
	{

	case dark_purple:
		return {
			accent[0],
			accent[1],
			accent[2],
			0.75f
		};

	case light_purple:
		return {
			accent[0] + 0.1f,
			accent[1] + 0.01f,
			accent[2] + 0.1f,
			0.75f
		};

	case light_purpleA:
		return {
			accent[0] + 0.6f,
			accent[1] + 0.51f,
			accent[2] + 0.6f,
			0.75f
		};

	case dark_grey:
		return {
			accent[0] + -0.1554902f,
			accent[1] + 0.0484313f,
			accent[2] + -0.3084314f,
			1.0f
		};

	default: // menu_border
		return {
			accent[0],
			accent[1],
			accent[2],
			accent[3]
		};

	}

	return {};
}