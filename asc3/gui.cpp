#include "pch.hpp"
#include "gui.hpp"
#include "input.hpp"
#include "weapon.hpp"
#include "config.hpp"
#include "entities.hpp"
#include "pScanning.hpp"
#include "GuiHelpers.hpp"

SwapWindowSig SwapWindow;

inline void RenderFrame()
{
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void SetupFrame()
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame();

	ImGui::NewFrame();
	ImGui::SetNextWindowSize({ 600, 450 }, ImGuiCond_FirstUseEver);

	ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetColor(MenuBorder));

	ImGui::Begin("sigmahack.cc", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
	ImGui::PopStyleColor();
}

void DrawMenu(SDL_Window* window)
{
	if (cfg.MenuOpen)
	{
		SetupFrame();

		if (ImGui::BeginTabBar("Tabs"))
		{
			
			if (ImGui::BeginTabItem("Weapon"))
			{
				ImGui::CheckboxEx("Aimbot", &cfg.aimbot);
				ImGui::Hotkey(cfg.aimkey, &cfg.vAimkey, 0);
				if (cfg.aimbot)
				{
					static const char* modes[]{ "FOV", "Distance", "Danger", "Health" };
					ImGui::ComboEx("##MODES", cfg.TargetMode, modes, IM_ARRAYSIZE(modes));
					ImGui::Description("targeting mode");

					ImGui::SliderFloatEx("##SMOOTHING", &cfg.smoothing, 0.0f, 100.0f);
					ImGui::Description("smoothing");

					ImGui::SliderFloatEx("##AIMBOT_FOV", &cfg.AimbotFOV, 0.5f, 360.0f);
					ImGui::Description("aimbot fov");
				}

				ImGui::CheckboxEx("Target on shoot", &cfg.TargetFire);

				ImGui::CheckboxEx("Target teammates", &cfg.TargetTeam);

				const bool StatusChange = ImGui::CheckboxEx("Adjust recoil", &cfg.AdjustRecoil);
				ImGui::Hotkey(cfg.recoilkey, &cfg.vRecoilkey, 1);
				if (cfg.AdjustRecoil)
				{
					static const char* modes[] = { "Visual", "Physical", "Both" };
					const bool ModeChange = ImGui::ComboEx("##RECOIL_MODE", cfg.RecoilMode, modes, IM_ARRAYSIZE(modes));
					ImGui::Description("recoil type");

					static float& recoil = cfg.RecoilSlider;
					const bool ValueChange = ImGui::SliderFloatEx("##RECOIL_SLIDER", &recoil, 0.0f, 100.0f);
					ImGui::Description("recoil percentage");

					if (ValueChange || ModeChange)
						SetRecoil(recoil);
				}
				else if (StatusChange) SetRecoil(100.0f);

				ImGui::CheckboxEx("Adjust spread", &cfg.AdjustSpread); // SHOTGUN SPREAD MUST BE ADDED
				ImGui::Hotkey(cfg.spreadkey, &cfg.vSpreadkey, 2);
				if (cfg.AdjustSpread)
				{
					static const char* modes[] = { "Regular", "Shotgun", "Both" };
					const bool ModeChange = ImGui::ComboEx("##SPREAD_MODE", cfg.SpreadMode, modes, IM_ARRAYSIZE(modes));
					ImGui::Description("spread type");

					static float spread = 100.0f;
					const bool ValueChange = ImGui::SliderFloatEx("##SPREAD_SLIDER", &spread, 0.0f, 100.0f);
					ImGui::Description("spread percentage");

					if (ValueChange || ModeChange)
					{
						switch (cfg.SpreadMode)
						{
						case regular: cfg.RegSpread     = spread; break;
						case shotgun: cfg.ShotgunSpread = spread; break;
						default:      cfg.RegSpread     = cfg.ShotgunSpread = spread;
						}
					}
				}

				ImGui::EndTabItem();
			}
			
			if (ImGui::BeginTabItem("Visuals"))
			{
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Exploits"))
			{
				if (ImGui::CheckboxEx("Force full auto", &cfg.ForceAuto))
				{
					const bool status = cfg.ForceAuto;
					LocalPlayer->pistol->weapon_info->is_auto = status;
					LocalPlayer->shotty->weapon_info->is_auto = status;
					LocalPlayer->sniper->weapon_info->is_auto = status;
				}
				ImGui::ToolTip("Removes the need to click every time you shoot with single fire weapons");

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Misc"))
			{
				if (ImGui::CheckboxEx("Adjust FOV", &cfg.AdjFOV))
				{
					if (cfg.AdjFOV) 
						*cfg.pFov = cfg.fov;
					else 
						*cfg.pFov = 90.0f;
				}
				if (cfg.AdjFOV)
				{
					if (ImGui::ResetButton(0))
						*cfg.pFov = cfg.fov = 90.0f;

					if (ImGui::SliderFloatEx("##FOV_SLIDER", &cfg.fov, 1.0f, 170.0f))
						*cfg.pFov = cfg.fov;
				}

				if (ImGui::CheckboxEx("Adjust roll", &cfg.AdjRoll))
				{
					if (cfg.AdjRoll) 
						LocalPlayer->roll = cfg.roll;
					else 
						LocalPlayer->roll = 0.0f;
				}
				if (cfg.AdjRoll)
				{
					if (ImGui::ResetButton(1))
						LocalPlayer->roll = cfg.roll = 0.0f;
						
					if (ImGui::SliderFloatEx("##ROLL_SLIDER", &cfg.roll, 0.0f, 360.0f))
						LocalPlayer->roll = cfg.roll;
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Settings"))
			{
				ImGui::CheckboxEx("Block game input", &cfg.BlockInput);
				ImGui::ToolTip("Blocks the game from handling input while the menu is open");

				ImGui::CheckboxEx("Block bound inputs", &cfg.BlockBinds);
				ImGui::ToolTip("Blocks the game from handling any input that is bound to a feature of the cheat");

				if (ImGui::ColorEditEx("Menu accent", cfg.MenuAccent))
				{
					static auto& colors = ImGui::GetStyle().Colors;

					colors[ImGuiCol_TabActive]        = ImGui::GetColor(LightPurple);
					colors[ImGuiCol_TabHovered]       = ImGui::GetColor(LightPurpleA);
					colors[ImGuiCol_SliderGrab]       = ImGui::GetColor(DarkPurple);
					colors[ImGuiCol_SliderGrabActive] = ImGui::GetColor(LightPurple);
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Config"))
			{
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		RenderFrame();
	}

	if (cfg.aimbot && *PlayerCount > 1 && !(cfg.TargetFire && !LocalPlayer->equiped_wpn->consecutive_shots)) aimbot();

	SwapWindow(window);
}

SDL_Window* InitGui()
{
	IMGUI_CHECKVERSION();

#ifdef _DEBUG

	if (!ImGui::CreateContext())
	{
		std::cout << "ImGui::CreateContext failed\n";
		return nullptr;
	}
	else std::cout << "ImGui::CreateContext success\n";

#else

	if (!ImGui::CreateContext())
		return nullptr;

#endif

	constexpr UINT16 WindowPtrn[] = 
	{
		0x68, 0x00, 0x00, 0xFF, 0x1F,   // push 0x1FFF0000            (int y)
		0x68, 0x00, 0x00, 0xFF, 0x1F,   // push 0x1FFF0000            (int x)
		0x68, unk, unk, unk, unk,       // push "AssaultCube"         (const char* title)
		0xFF, 0x15, unk, unk, unk, unk, // call ds:SDL_CreateWindow   (__cdecl)
		0x8B, 0xC8,                     // mov ecx, eax               (eax = SDL_Window*)
		0x83, 0xC4, 0x20                // add esp, 0x20
	};
	constexpr UINT16 ContextPtrn[] =
	{
		0x51,                           // push ecx                       (ecx = SDL_Window*)
		0xFF, 0x15, unk, unk, unk, unk, // call ds:SDL_GL_CreateContext   (__cdecl)
		0x83, 0xC4, 0x04,               // add esp, 4
		0xA3, unk, unk, unk, unk,       // mov SDL_GLContext, eax         (eax = SDL_GLContext*)
		0x85, 0xC0                      // test eax, eax
	};
	
	const auto pWindow = reinterpret_cast<SDL_Window**>(ResolveAddress(WindowPtrn, PtrnSz(WindowPtrn), 28));

#ifdef _DEBUG

	if (!pWindow)
	{
		std::cout << "Failed to resolve WindowPtrn\n";
		return nullptr;
	}
	else std::cout << "Resolved WindowPtrn\n";

#else

	if (!pWindow) return nullptr;

#endif

	const auto pContext = reinterpret_cast<SDL_GLContext*>(ResolveAddress(ContextPtrn, PtrnSz(ContextPtrn), 11));

#ifdef _DEBUG

	if (!pContext)
	{
		std::cout << "Failed to resolve ContextPtrn\n";
		return nullptr;
	}
	else std::cout << "Resolved ContextPtrn\n";

#else

	if (!pContext) return nullptr;

#endif

#ifdef _DEBUG

	if (!ImGui_ImplSDL2_InitForOpenGL(*pWindow, *pContext))
	{
		std::cout << "ImGui_ImplSDL2_InitForOpenGL failed\n";
		return nullptr;
	}
	else std::cout << "ImGui_ImplSDL2_InitForOpenGL success\n";

	if (!ImGui_ImplOpenGL2_Init())
	{
		std::cout << "ImGui_ImplOpenGL2_Init failed\n";
		return nullptr;
	}
	else std::cout << "ImGui_ImplOpenGL2_Init success\n";

#else

	if (!ImGui_ImplSDL2_InitForOpenGL(*pWindow, *pContext))
		return nullptr;

	if (!ImGui_ImplOpenGL2_Init())
		return nullptr;

#endif

	ImGuiStyle& style = ImGui::GetStyle();
	
	constexpr ImVec4 clear          = { 0.0f, 0.0f, 0.0f, 0.0f };
	constexpr ImVec4 black          = { 0.0f, 0.0f, 0.0f, 1.0f };
	constexpr ImVec4 grey           = { 0.0901960f, 0.0901960f, 0.0901960f, 1.0f };
	constexpr ImVec4 DarkGreyC      = { 0.0601960f, 0.0601960f, 0.0601960f, 1.0f };
	constexpr ImVec4 LightGrey      = { 0.1562745f, 0.1562745f, 0.1562745f, 1.0f };
	constexpr ImVec4 DarkGrey_Child = { 0.0501960f, 0.0501960f, 0.0501960f, 1.0f };
	constexpr ImVec4 DarkPurpleC    = { 0.2156862f, 0.0117647f, 0.3686274f, 0.75f };
	constexpr ImVec4 LightPurpleC   = { 0.3156862f, 0.0217647f, 0.4686274f, 0.75f };
	constexpr ImVec4 LightPurple2   = { 0.4156862f, 0.0317647f, 0.5686274f, 0.75f };

	style.TabRounding     = 0.0f;
	style.FrameBorderSize = 1.0f;
	style.FramePadding    = { 7.0f, 7.0f };
	style.Colors[ImGuiCol_ResizeGrip]        = clear;
	style.Colors[ImGuiCol_ResizeGripActive]  = clear;
	style.Colors[ImGuiCol_ResizeGripHovered] = clear;
	style.Colors[ImGuiCol_TitleBgActive]     = black;
	style.Colors[ImGuiCol_TitleBg]           = black;
	style.Colors[ImGuiCol_WindowBg]          = grey;
	style.Colors[ImGuiCol_Button]            = DarkGreyC;
	style.Colors[ImGuiCol_ButtonActive]      = LightGrey;
	style.Colors[ImGuiCol_ButtonHovered]     = LightGrey;
	style.Colors[ImGuiCol_PopupBg]           = DarkGreyC;
	style.Colors[ImGuiCol_Header]            = DarkGreyC;
	style.Colors[ImGuiCol_HeaderActive]      = LightGrey;
	style.Colors[ImGuiCol_HeaderHovered]     = LightGrey;
	style.Colors[ImGuiCol_FrameBg]           = DarkGreyC;
	style.Colors[ImGuiCol_FrameBgHovered]    = LightGrey;
	style.Colors[ImGuiCol_FrameBgActive]     = LightGrey;
	style.Colors[ImGuiCol_ChildBg]           = DarkGrey_Child;
	style.Colors[ImGuiCol_CheckMark]         = black;
	style.Colors[ImGuiCol_Tab]               = DarkGreyC;
	style.Colors[ImGuiCol_TabActive]         = LightPurpleC;
	style.Colors[ImGuiCol_TabHovered]        = LightPurple2;
	style.Colors[ImGuiCol_SliderGrab]        = DarkPurpleC;
	style.Colors[ImGuiCol_SliderGrabActive]  = LightPurpleC;
	style.Colors[ImGuiCol_Separator]         = DarkGreyC;

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigWindowsResizeFromEdges = false;

	io.Fonts->AddFontFromFileTTF("c:\\Aileron.ttf", 16);
	io.FontDefault = io.Fonts->Fonts[0];

	return *pWindow;
}