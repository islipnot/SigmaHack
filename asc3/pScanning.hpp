#pragma once

#define unk 0xFFFF // unknown

#define PtrnSz(pattern) sizeof(pattern) / sizeof(UINT16)

BYTE* ResolveAddress(const UINT16* pattern, UINT pattern_size, int return_offset = 0, bool deref = true);