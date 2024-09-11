#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <Psapi.h>

#ifdef _DEBUG
	#include <iostream>
#endif

#include <string>

#include <SDL.h>
#include <SDL_syswm.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl2.h>