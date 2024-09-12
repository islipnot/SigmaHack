#define dark_purple   0
#define light_purple  1
#define light_purpleA 2
#define dark_grey     3
#define menu_border   4

namespace ImGui
{
	void Description(const char* const desc);

	bool CheckboxEx(const char* const label, bool* const var);

	bool SliderFloatEx(const char* const label, float* const var, const float min, const float max);

	bool ComboEx(const char* const label, int& selection, const char* const items[], const int item_count);

	bool ResetButton(const int item_id);

	void ToolTip(const char* const text);

	bool ColorEditEx(const char* const label, float col[4]);

	void Hotkey(char buffer[2], int* const vBuffer, const int ID);

	ImVec4 GetColor(const int original_color);
}