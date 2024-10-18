#pragma once

// Macros

#define unk 0x100 // unknown

#define PtrnSz(pattern) sizeof(pattern) / sizeof(UINT16)

#define ResolveAndDerefAddress(pattern, PtrnSize, RetOffset) *reinterpret_cast<void**>(ResolveAddress(pattern, PtrnSize, RetOffset))

// Forward declarations

BYTE* ResolveAddress(const UINT16* const pattern, UINT PtrnSize, int RetOffset = 0);

bool InitCheat(SDL_Window* const window);

SDL_Window* InitGui();

// Templates

template <typename ret = void*> auto ResolveAddressEx(const UINT16* const pattern, UINT PtrnSize, int RetOffset = 0) -> ret
{
	return reinterpret_cast<ret>(ResolveAndDerefAddress(pattern, PtrnSize, RetOffset));
}