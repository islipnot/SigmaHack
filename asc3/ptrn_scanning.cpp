#include "pch.h"
#include "pScanning.hpp"

BYTE* ResolveAddress(const BYTE pattern[], int PatternSz, BYTE RetOffset, bool deref)
{
	static HMODULE hModule = GetModuleHandle(L"ac_client.exe");
	static MODULEINFO ModuleInfo{ NULL };

	if (!ModuleInfo.lpBaseOfDll) 
		GetModuleInformation(GetCurrentProcess(), hModule, &ModuleInfo, sizeof(MODULEINFO));

	auto pModule = reinterpret_cast<BYTE*>(ModuleInfo.lpBaseOfDll);
	const BYTE* ScanEnd = (pModule + ModuleInfo.SizeOfImage) - PatternSz;

	for (;pModule < ScanEnd; ++pModule)
	{
		if (pattern[0] == *pModule)
		{
			BYTE* ResolvedBase = pModule;
			++pModule;

			for (int x = 1; x < PatternSz; ++pModule, ++x)
			{
				if (pattern[x] == unk) continue;
				if (pattern[x] != *pModule)
				{
					ResolvedBase = nullptr;
					break;
				}
			}

			if (ResolvedBase != nullptr)
			{
				BYTE* const ResolvedAddr = ResolvedBase + RetOffset;
				return deref ? *reinterpret_cast<BYTE**>(ResolvedAddr) : ResolvedAddr;
			}
		}
	}

	return nullptr; 
}