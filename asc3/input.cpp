#include "pch.h"
#include "input.hpp"

menu_cfg cfg;
WndProc_sig og_wndproc;
PollEvents_sig PollEvents;

int HandleEvent(SDL_Event* sdl_event)
{
	const int status = PollEvents(sdl_event);

	if (cfg.menu_open)
	{
		ImGui_ImplSDL2_ProcessEvent(sdl_event);
		if (cfg.block_input) return 0;
	}

	return status;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_KEYDOWN)
	{
		if (wParam == VK_INSERT)
		{
			SDL_SetRelativeMouseMode(static_cast<SDL_bool>(cfg.menu_open));

			cfg.menu_open = !cfg.menu_open;
			ImGui::GetIO().WantCaptureMouse = cfg.menu_open;
			ImGui::GetIO().WantCaptureKeyboard = cfg.menu_open;

			return 1;
		}

		else if (wParam == cfg.vAimkey)
		{
			cfg.aimbot = !cfg.aimbot;
			if (cfg.block_binds) return 1;
		}

		else if (wParam == cfg.vRecoilkey)
		{
			cfg.adjust_recoil = !cfg.adjust_recoil;

			if (!cfg.adjust_recoil) SetRecoil(100.0f);
			else SetRecoil(cfg.recoil_slider);
			
			if (cfg.block_binds) return 1;
		}

		else if (wParam == cfg.vSpreadkey)
		{
			cfg.adjust_spread = !cfg.adjust_spread;
			if (cfg.block_binds) return 1;
		}
	} 

	return og_wndproc(hWnd, msg, wParam, lParam);
}