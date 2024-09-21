// Targeting modes
#define ClosestFOV 0  // targets whoever is closest to the center of FOV circle
#define closest     1  // targets whoever is closest to the player
#define danger      2  // targets whoever is most likely to kill the user
#define lowest      3  // targets whoever is the lowest health

// Recoil modes
#define visual   0
#define physical 1

// Spread modes
#define regular 0
#define shotgun 1

struct MenuCfg
{
	// Weapon
	int TargetMode       = ClosestFOV;
	int RecoilMode       = visual;
	int SpreadMode       = regular;
	bool aimbot          = false;
	bool TargetFire      = true;
	bool TargetTeam      = false;
	bool AdjustRecoil    = false;
	bool AdjustSpread    = false;
	float VisRecoilMulti = 0.1f;
	float VisRecoil      = 100.0f;
	float PhysRecoil     = 100.0f;
	float RegSpread      = 100.0f;
	float ShotgunSpread  = 100.0f;
	float AimbotFOV      = 90.0f;
	float smoothing      = 0.0f;
	float RecoilSlider   = 100.0f;

	// Visuals
	bool skeletons  = false;
	bool boxes      = false;
	bool snaplines  = false;
	bool HealthBar  = false;

	// Exploits
	bool ForceAuto = false;

	// Misc
	bool AdjFOV   = false;
	bool AdjRoll  = false;
	float* pFov   = nullptr;
	float fov     = 90.0f;
	float roll    = 0.0f;

	// Settings
	bool MenuOpen       = true;
	bool BlockInput     = true;
	bool BlockBinds     = false;
	float MenuAccent[4] = { 0.2156862f, 0.0117647f, 0.3686274f, 0.85f }; // dark purple

	// Binds
	char aimkey[2]    = "\0";
	char recoilkey[2] = "\0";
	char spreadkey[2] = "\0";
	int vAimkey       = NULL;
	int vRecoilkey    = NULL;
	int vSpreadkey    = NULL;
};

extern MenuCfg cfg;