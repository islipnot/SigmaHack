#pragma once

// Macros

#define unk 0x100 // unknown

#define PtrnSz(pattern) sizeof(pattern) / sizeof(UINT16)

// Forward declarations

BYTE* ResolveAddress(const UINT16* const pattern, UINT pattern_size, int return_offset = 0); 

void* ResolveAndDerefAddress(const UINT16* const pattern, UINT pattern_size, int return_offset = 0);

// Templates

template <typename ret> auto ResolveAddressEx(const UINT16* const pattern, UINT PtrnSz, int RetOffset = 0) -> ret
{
	return reinterpret_cast<ret>(ResolveAndDerefAddress(pattern, PtrnSz, RetOffset));
}