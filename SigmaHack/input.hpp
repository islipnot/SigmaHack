#pragma once

// Forward declarations

int HandleEvent(SDL_Event* sdl_event);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Typedefs

typedef LRESULT(CALLBACK* WndProcSig)(HWND, UINT, WPARAM, LPARAM);

typedef int(*PollEventsSig)(SDL_Event*);