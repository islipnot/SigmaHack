#include "weapon.hpp"

int HandleEvent(SDL_Event* sdl_event);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef LRESULT(CALLBACK* WndProc_sig)(HWND, UINT, WPARAM, LPARAM);

typedef int(*PollEvents_sig)(SDL_Event*);

struct menu_cfg
{
	// Weapon
	int target_mode      = closest_fov;
	int recoil_mode      = visual;
	int spread_mode      = regular;
	bool aimbot          = false;
	bool target_fire     = true;
	bool target_team     = false;
	bool adjust_recoil   = false;
	bool adjust_spread   = false;
	float vis_recoil_mlt = 0.1f;
	float vis_recoil     = 100.0f;
	float phys_recoil    = 100.0f;
	float reg_spread     = 100.0f;
	float shotgun_spread = 100.0f;
	float aimbot_fov     = 90.0f;
	float smoothing      = 0.0f;

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
	bool menu_open       = true;
	bool block_input     = true;
	float menu_accent[4] = { 0.2156862f, 0.0117647f, 0.3686274f, 0.85f }; // dark purple

	// Binds
	char aimkey[2] = "\0";
	int vAimkey    = NULL;
};

extern menu_cfg cfg;