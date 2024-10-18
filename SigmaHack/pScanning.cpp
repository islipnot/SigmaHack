#include "pch.hpp"
#include "pScanning.hpp"

BYTE* ResolveAddress(const UINT16* const pattern, const UINT PatternSz, const int RetOffset)
{
	static HMODULE hModule = GetModuleHandle(L"ac_client.exe");
	static MODULEINFO ModuleInfo{ nullptr };

	if (!ModuleInfo.lpBaseOfDll) GetModuleInformation(GetCurrentProcess(), hModule, &ModuleInfo, sizeof(MODULEINFO));

	BYTE* base = static_cast<BYTE*>(ModuleInfo.lpBaseOfDll);

	for (const BYTE* end = (base + ModuleInfo.SizeOfImage) - PatternSz; base < end; ++base)
	{
		UINT MatchingBytes = 0;

		for (UINT i = 0; i < PatternSz; ++i, ++base, ++MatchingBytes)
		{
			if (!(pattern[i] & unk) && static_cast<BYTE>(pattern[i]) != *base)
				break;
		}

		if (MatchingBytes == PatternSz)
		{
			return (base - MatchingBytes) + RetOffset;
		}
	}

	return nullptr; 
}

void* ResolveAndDerefAddress(const UINT16* const pattern, const UINT PatternSz, const int RetOffset)
{
	return *reinterpret_cast<void**>(ResolveAddress(pattern, PatternSz, RetOffset));
}