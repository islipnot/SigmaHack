#include "pch.h"
#include "input.hpp"
#include "config.hpp"
#include "weapon.hpp"

MenuCfg cfg;
WndProcSig ogWndProc;
PollEventsSig PollEvents;

int HandleEvent(SDL_Event* sdl_event)
{
	const int status = PollEvents(sdl_event);

	if (cfg.MenuOpen)
	{
		ImGui_ImplSDL2_ProcessEvent(sdl_event);
		if (cfg.BlockInput) return 0;
	}

	return status;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_KEYDOWN)
	{
		if (wParam == VK_INSERT)
		{
			SDL_SetRelativeMouseMode(static_cast<SDL_bool>(cfg.MenuOpen));
			cfg.MenuOpen = !cfg.MenuOpen;

			static auto& io = ImGui::GetIO();
			io.WantCaptureMouse    = cfg.MenuOpen;
			io.WantCaptureKeyboard = cfg.MenuOpen;

			return 1;
		}

		else if (wParam == cfg.vAimkey)
		{
			cfg.aimbot = !cfg.aimbot;
			if (cfg.BlockBinds) return 1;
		}

		else if (wParam == cfg.vRecoilkey)
		{
			cfg.AdjustRecoil = !cfg.AdjustRecoil;

			if (!cfg.AdjustRecoil) SetRecoil(100.0f);
			else SetRecoil(cfg.RecoilSlider);
			
			if (cfg.BlockBinds) return 1;
		}

		else if (wParam == cfg.vSpreadkey)
		{
			cfg.AdjustSpread = !cfg.AdjustSpread;
			if (cfg.BlockBinds) return 1;
		}
	} 

	return ogWndProc(hWnd, msg, wParam, lParam);
}