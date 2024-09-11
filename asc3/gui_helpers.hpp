#define dark_purple   0
#define light_purple  1
#define light_purpleA 2
#define dark_grey     3
#define menu_border   4

bool CheckboxEx(const char* const label, bool* const var);

int SliderFloatEx(const char* label, float* var, const float min, const float max);

bool ComboEx(const char* label, int* selection, const char* items[], int item_count);

bool ResetButton(const int item_id);

void ToolTip(const char* const text);

bool ColorEditEx(const char* label, float col[4]);

ImVec4 GetColor(const int original_color);