#define unk 255 // unknown - in place of bytes that are subject to change

BYTE* ResolveAddress(const BYTE pattern[], int pattern_size, BYTE return_offset = 0, bool deref = true);