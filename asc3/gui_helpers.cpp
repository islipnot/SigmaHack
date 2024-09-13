#include "pch.h"
#include "input.hpp"
#include "config.hpp"
#include "gui_helpers.hpp"

namespace ImGui
{
	void Description(const char* desc)
	{
		ImGui::SameLine();
		ImGui::TextColored({ 1.0f, 1.0f, 1.0f, 0.50f }, desc);
	}

	bool CheckboxEx(const char* label, bool* var)
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

	bool SliderFloatEx(const char* label, float* var, float min, float max)
	{
		static const auto& style = ImGui::GetStyle();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { style.FramePadding.x, 4.0f });
		ImGui::SetNextItemWidth(250.0f);

		const bool status = ImGui::SliderFloat(label, var, min, max);
		ImGui::PopStyleVar();

		return status;
	}

	bool ComboEx(const char* label, int& selection, const char* items[], int item_count)
	{
		static const auto& style = ImGui::GetStyle();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { style.FramePadding.x, 4.0f });
		ImGui::PushItemWidth(250.0f);

		bool changed = false;

		if (ImGui::BeginCombo(label, items[selection]))
		{
			for (int i = 0; i < item_count; ++i)
			{
				const bool is_selected = selection == i;

				if (is_selected)
				{
					constexpr ImVec4 green       = { 0.058824f, 0.309804f, 0.105882f, 1.0f };
					constexpr ImVec4 light_green = { 0.068824f, 0.409804f, 0.205882f, 1.0f };

					ImGui::PushStyleColor(ImGuiCol_Header,        green);
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, light_green);
					ImGui::PushStyleColor(ImGuiCol_HeaderActive,  light_green);
				}

				if (ImGui::Selectable(items[i], is_selected) && selection != i)
				{
					selection = i;
					changed = true;
				}

				if (is_selected)
				{
					ImGui::PopStyleColor(3);
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();

		return changed;
	}

	bool ResetButton(int item_id)
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

	void ToolTip(const char* text)
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

	void Hotkey(char buffer[2], int* vBuffer, int ID)
	{
		ImGui::SameLine();
		ImGui::Text("-->");
		ImGui::SameLine();

		constexpr ImVec4 light_grey = { 0.1562745f, 0.1562745f, 0.1562745f, 1.0f };
		ImGui::PushStyleColor(ImGuiCol_Border, light_grey);

		static const float width = ImGui::CalcTextSize("text").x;
		ImGui::SetNextItemWidth(width);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 6.0f, 3.0f });
		ImGui::PushID(ID);

		constexpr int flags = ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CharsUppercase;

		if (ImGui::InputText("##KEYBIND", buffer, IM_ARRAYSIZE(cfg.aimkey), flags))
			*vBuffer = static_cast<int>(LOBYTE(VkKeyScanA(buffer[0])));

		ImGui::PopID();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::Description("hotkey");
	}

	ImVec4 GetColor(int original_color)
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
}