#define dark_purple   0
#define light_purple  1
#define light_purpleA 2
#define dark_grey     3
#define menu_border   4

namespace ImGui
{
	void Description(const char* desc);

	bool CheckboxEx(const char* label, bool* var);

	bool SliderFloatEx(const char* label, float* var, float min, float max);

	bool ComboEx(const char* label, int& selection, const char* items[], int item_count);

	bool ResetButton(int item_id);

	void ToolTip(const char* text);

	bool ColorEditEx(const char* label, float col[4]);

	void Hotkey(char buffer[2], int* vBuffer, int ID);

	ImVec4 GetColor(int original_color);
}