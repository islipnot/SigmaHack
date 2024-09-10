#include "pch.h"
#include "pScanning.hpp"

BYTE* ResolveAddress(const BYTE pattern[], const int pattern_size, const BYTE return_offset, const bool deref)
{
	static auto ac_client_base = (BYTE*)GetModuleHandle(L"ac_client.exe");

	static MODULEINFO module_info{ NULL };
	if (!module_info.lpBaseOfDll) GetModuleInformation(GetCurrentProcess(), HMODULE(ac_client_base), &module_info, sizeof(MODULEINFO));

	auto module_base = (BYTE*)module_info.lpBaseOfDll;
	const BYTE* scan_end = (module_base + module_info.SizeOfImage) - pattern_size;

	for (BYTE* cur_module_pos = module_base; cur_module_pos < scan_end; ++cur_module_pos)
	{
		if (pattern[0] == *cur_module_pos)
		{
			BYTE* resolved_base = cur_module_pos;
			++cur_module_pos;

			for (int x = 1; x < pattern_size; ++cur_module_pos, ++x)
			{
				if (pattern[x] == unk) continue;
				if (pattern[x] != *cur_module_pos)
				{
					resolved_base = nullptr;
					break;
				}
			}

			if (resolved_base != nullptr)
			{
				BYTE* resolved_address = (resolved_base + (BYTE)return_offset);
				if (deref) return *(BYTE**)resolved_address;
				else return resolved_address;
			}
		}
	}

	return nullptr; 
}