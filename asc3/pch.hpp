#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <Psapi.h>

#ifdef _DEBUG
	#include <iostream>
#endif

#include <string>

// Suppressing unneeded warnings from included libs

#pragma warning(push)
#pragma warning(disable : 4109)  // imgui.h
#pragma warning(disable : 4161)  // imgui.h
#pragma warning(disable : 26819) // SDL_stdinc.h

#include <SDL.h>
#include <SDL_syswm.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl2.h>

#pragma warning(pop)