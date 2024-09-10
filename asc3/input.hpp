#define closest_fov 0  // targets whoever is closest to the center of FOV circle
#define closest     1  // targets whoever is closest to the player
#define danger      2  // targets whoever is most likely to kill the user
#define lowest      3  // targets whoever is the lowest health

int HandleEvent(SDL_Event* sdl_event);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef LRESULT(CALLBACK* WndProc_sig)(HWND, UINT, WPARAM, LPARAM);

typedef int(*PollEvents_sig)(SDL_Event*);

struct menu_cfg
{
	// Weapon
	int target_mode  = closest;
	bool aimbot      = false;
	bool target_team = false;
	bool adjust_recoil = false;
	bool adjust_spread = false;
	float vis_recoil_mlt = 0.1f;
	float vis_recoil     = 100.0f;
	float phys_recoil    = 100.0f;
	float reg_spread     = 100.0f;
	float shotgun_spread = 100.0f;
	float smoothing = 0.0f;

	// Visuals
	bool skeletons  = false;
	bool boxes      = false;
	bool snaplines  = false;
	bool health_bar = false;

	// Exploits
	bool force_auto = false;
	

	// Misc
	bool adj_fov  = false;
	bool adj_roll = false;
	float* pFov   = nullptr;
	float fov     = 90.0f;
	float roll    = 0.0f;

	// Theme
	bool menu = true;
	bool block_input = true;
};