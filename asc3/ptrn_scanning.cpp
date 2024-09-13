#include "pch.h"
#include "pScanning.hpp"

BYTE* ResolveAddress(const BYTE pattern[], int pattern_size, BYTE return_offset, bool deref)
{
	static HMODULE ac_client_base = GetModuleHandle(L"ac_client.exe");
	static MODULEINFO module_info{ NULL };

	if (!module_info.lpBaseOfDll) 
		GetModuleInformation(GetCurrentProcess(), ac_client_base, &module_info, sizeof(MODULEINFO));

	auto module_ptr = reinterpret_cast<BYTE*>(module_info.lpBaseOfDll);
	const BYTE* scan_end = (module_ptr + module_info.SizeOfImage) - pattern_size;

	for (;module_ptr < scan_end; ++module_ptr)
	{
		if (pattern[0] == *module_ptr)
		{
			BYTE* resolved_base = module_ptr;
			++module_ptr;

			for (int x = 1; x < pattern_size; ++module_ptr, ++x)
			{
				if (pattern[x] == unk) continue;
				if (pattern[x] != *module_ptr)
				{
					resolved_base = nullptr;
					break;
				}
			}

			if (resolved_base != nullptr)
			{
				BYTE* const resolved_address = resolved_base + return_offset;

				if (deref) 
					return *reinterpret_cast<BYTE**>(resolved_address);

				return resolved_address;
			}
		}
	}

	return nullptr; 
}