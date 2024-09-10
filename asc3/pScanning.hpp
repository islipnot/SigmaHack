#define unk 255 // unknown - in place of bytes that are subject to change

BYTE* ResolveAddress(const BYTE pattern[], const int pattern_size, const BYTE return_offset = 0, const bool deref = true);