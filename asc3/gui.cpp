#include "pch.h"
#include "gui.hpp"
#include "input.hpp"
#include "weapon.hpp"
#include "entities.hpp"
#include "pScanning.hpp"
#include "gui_helpers.hpp"

SwapWindow_sig SwapWindow;

static inline void RenderFrame()
{
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

static void SetupFrame()
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame();

	ImGui::NewFrame();
	ImGui::SetNextWindowSize({ 600, 450 }, ImGuiCond_FirstUseEver);

	ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetColor(menu_border));

	ImGui::Begin("sigmahack.cc", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
	ImGui::PopStyleColor();
}

void DrawMenu(SDL_Window* window)
{
	if (cfg.menu_open)
	{
		SetupFrame();

		if (ImGui::BeginTabBar("Tabs"))
		{
			
			if (ImGui::BeginTabItem("Weapon"))
			{
				ImGui::CheckboxEx("Aimbot", &cfg.aimbot);
				ImGui::BindBox(cfg.aimkey, &cfg.vAimkey, 0);
				if (cfg.aimbot)
				{
					static const char* modes[]{ "FOV", "Distance", "Danger", "Health" };
					ImGui::ComboEx("##MODES", cfg.target_mode, modes, IM_ARRAYSIZE(modes));
					ImGui::Description("targeting mode");

					ImGui::SliderFloatEx("##SMOOTHING", &cfg.smoothing, 0.0f, 100.0f);
					ImGui::Description("smoothing");

					ImGui::SliderFloatEx("##AIMBOT_FOV", &cfg.aimbot_fov, 0.5f, 360.0f);
					ImGui::Description("aimbot fov");
				}

				ImGui::CheckboxEx("Target on shoot", &cfg.target_fire);

				ImGui::CheckboxEx("Target teammates", &cfg.target_team);

				ImGui::CheckboxEx("Adjust recoil", &cfg.adjust_recoil);
				if (cfg.adjust_recoil)
				{
					static const char* recoil_types[] = { "Visual", "Physical", "Both" };
					const bool mode_changed = ImGui::ComboEx("##RECOIL_MODE", cfg.recoil_mode, recoil_types, IM_ARRAYSIZE(recoil_types));
					ImGui::Description("recoil type");

					static float recoil = 100.0f;
					const bool value_changed = ImGui::SliderFloatEx("##RECOIL_SLIDER", &recoil, 0.0f, 100.0f);
					ImGui::Description("recoil percentage");

					if (value_changed || mode_changed)
					{
						switch (cfg.recoil_mode)
						{
						case visual:   { cfg.vis_recoil  = recoil; cfg.vis_recoil_mlt = recoil / 1000.0f; break; }
						case physical: { cfg.phys_recoil = recoil; break; }
						default:       { cfg.vis_recoil  = cfg.phys_recoil = recoil; }
						}
					}
				}

				ImGui::CheckboxEx("Adjust spread", &cfg.adjust_spread); // SHOTGUN SPREAD MUST BE ADDED
				if (cfg.adjust_spread)
				{
					static const char* spread_types[] = { "Regular", "Shotgun", "Both" };
					const bool mode_changed = ImGui::ComboEx("##SPREAD_MODE", cfg.spread_mode, spread_types, IM_ARRAYSIZE(spread_types));
					ImGui::Description("spread type");

					static float spread = 100.0f;
					const bool value_changed = ImGui::SliderFloatEx("##SPREAD_SLIDER", &spread, 0.0f, 100.0f);
					ImGui::Description("spread percentage");

					if (value_changed || mode_changed)
					{
						switch (cfg.spread_mode)
						{
						case regular: { cfg.reg_spread     = spread; break; }
						case shotgun: { cfg.shotgun_spread = spread; break; }
						default:      { cfg.reg_spread     = cfg.shotgun_spread = spread; }
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
				if (ImGui::CheckboxEx("Force full auto", &cfg.force_auto))
				{
					const bool status = cfg.force_auto;
					local_player->pistol->weapon_info->is_auto = status;
					local_player->shotty->weapon_info->is_auto = status;
					local_player->sniper->weapon_info->is_auto = status;
				}
				ImGui::ToolTip("Removes the need to click every time you shoot with single fire weapons");

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Misc"))
			{
				if (ImGui::CheckboxEx("Adjust FOV", &cfg.adj_fov))
				{
					if (cfg.adj_fov) 
						*cfg.pFov = cfg.fov;
					else 
						*cfg.pFov = 90.0f;
				}
				if (cfg.adj_fov)
				{
					if (ImGui::ResetButton(0))
						*cfg.pFov = cfg.fov = 90.0f;

					if (ImGui::SliderFloatEx("##FOV_SLIDER", &cfg.fov, 1.0f, 170.0f))
						*cfg.pFov = cfg.fov;
				}

				if (ImGui::CheckboxEx("Adjust roll", &cfg.adj_roll))
				{
					if (cfg.adj_roll) 
						local_player->roll = cfg.roll;
					else 
						local_player->roll = 0.0f;
				}
				if (cfg.adj_roll)
				{
					if (ImGui::ResetButton(1))
						local_player->roll = cfg.roll = 0.0f;
						
					if (ImGui::SliderFloatEx("##ROLL_SLIDER", &cfg.roll, 0.0f, 360.0f))
						local_player->roll = cfg.roll;
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Theme"))
			{
				ImGui::CheckboxEx("Block game input", &cfg.block_input);

				if (ImGui::ColorEditEx("Menu accent", cfg.menu_accent))
				{
					static auto& colors = ImGui::GetStyle().Colors;

					colors[ImGuiCol_TabActive]        = ImGui::GetColor(light_purple);
					colors[ImGuiCol_TabHovered]       = ImGui::GetColor(light_purpleA);
					colors[ImGuiCol_SliderGrab]       = ImGui::GetColor(dark_purple);
					colors[ImGuiCol_SliderGrabActive] = ImGui::GetColor(light_purple);
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

	if (cfg.aimbot && *player_count > 1 && !(cfg.target_fire && !local_player->equiped_wpn->consecutive_shots)) aimbot();

	SwapWindow(window);
}

SDL_Window* InitGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	constexpr BYTE window_pattern[] = 
	{
		0x68, 0x00, 0x00, 0xFF, 0x1F,   // push 0x1FFF0000            (int y)
		0x68, 0x00, 0x00, 0xFF, 0x1F,   // push 0x1FFF0000            (int x)
		0x68, unk, unk, unk, unk,       // push "AssaultCube"         (const char* title)
		0xFF, 0x15, unk, unk, unk, unk, // call ds:SDL_CreateWindow   (__cdecl)
		0x8B, 0xC8,                     // mov ecx, eax               (eax = SDL_Window*)
		0x83, 0xC4, 0x20                // add esp, 0x20
	};
	constexpr BYTE context_pattern[] =
	{
		0x51,                           // push ecx                       (ecx = SDL_Window*)
		0xFF, 0x15, unk, unk, unk, unk, // call ds:SDL_GL_CreateContext   (__cdecl)
		0x83, 0xC4, 0x04,               // add esp, 4
		0xA3, unk, unk, unk, unk,       // mov SDL_GLContext, eax         (eax = SDL_GLContext*)
		0x85, 0xC0                      // test eax, eax
	};
	
	const auto window_ptr = (SDL_Window**)ResolveAddress(window_pattern, sizeof(window_pattern), 28);
	const auto context_ptr = (SDL_GLContext*)ResolveAddress(context_pattern, sizeof(context_pattern), 11);

	ImGui_ImplSDL2_InitForOpenGL(*window_ptr, *context_ptr);
	ImGui_ImplOpenGL2_Init();

	ImGuiStyle& style = ImGui::GetStyle();
	
	constexpr ImVec4 clear          = { 0.0f, 0.0f, 0.0f, 0.0f };
	constexpr ImVec4 black          = { 0.0f, 0.0f, 0.0f, 1.0f };
	constexpr ImVec4 grey           = { 0.0901960f, 0.0901960f, 0.0901960f, 1.0f };
	constexpr ImVec4 DarkGrey       = { 0.0601960f, 0.0601960f, 0.0601960f, 1.0f };
	constexpr ImVec4 LightGrey      = { 0.1562745f, 0.1562745f, 0.1562745f, 1.0f };
	constexpr ImVec4 DarkGrey_Child = { 0.0501960f, 0.0501960f, 0.0501960f, 1.0f };
	constexpr ImVec4 DarkPurple     = { 0.2156862f, 0.0117647f, 0.3686274f, 0.75f };
	constexpr ImVec4 LightPurple    = { 0.3156862f, 0.0217647f, 0.4686274f, 0.75f };
	constexpr ImVec4 LightPurple_2  = { 0.4156862f, 0.0317647f, 0.5686274f, 0.75f };

	style.TabRounding     = 0.0f;
	style.FrameBorderSize = 1.0f;
	style.FramePadding    = { 7.0f, 7.0f };
	style.Colors[ImGuiCol_ResizeGrip]        = clear;
	style.Colors[ImGuiCol_ResizeGripActive]  = clear;
	style.Colors[ImGuiCol_ResizeGripHovered] = clear;
	style.Colors[ImGuiCol_TitleBgActive]     = black;
	style.Colors[ImGuiCol_TitleBg]           = black;
	style.Colors[ImGuiCol_WindowBg]          = grey;
	style.Colors[ImGuiCol_Button]            = DarkGrey;
	style.Colors[ImGuiCol_ButtonActive]      = LightGrey;
	style.Colors[ImGuiCol_ButtonHovered]     = LightGrey;
	style.Colors[ImGuiCol_PopupBg]           = DarkGrey;
	style.Colors[ImGuiCol_Header]            = DarkGrey;
	style.Colors[ImGuiCol_HeaderActive]      = LightGrey;
	style.Colors[ImGuiCol_HeaderHovered]     = LightGrey;
	style.Colors[ImGuiCol_FrameBg]           = DarkGrey;
	style.Colors[ImGuiCol_FrameBgHovered]    = LightGrey;
	style.Colors[ImGuiCol_FrameBgActive]     = LightGrey;
	style.Colors[ImGuiCol_ChildBg]           = DarkGrey_Child;
	style.Colors[ImGuiCol_CheckMark]         = black;
	style.Colors[ImGuiCol_Tab]               = DarkGrey;
	style.Colors[ImGuiCol_TabActive]         = LightPurple;
	style.Colors[ImGuiCol_TabHovered]        = LightPurple_2;
	style.Colors[ImGuiCol_SliderGrab]        = DarkPurple;
	style.Colors[ImGuiCol_SliderGrabActive]  = LightPurple;
	style.Colors[ImGuiCol_Separator]         = DarkGrey;

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigWindowsResizeFromEdges = false;

	io.Fonts->AddFontFromFileTTF("c:\\aileron.ttf", 16);
	io.FontDefault = io.Fonts->Fonts[0];

	return *window_ptr;
}