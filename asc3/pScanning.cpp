#include "pch.hpp"
#include "pScanning.hpp"

BYTE* ResolveAddress(const UINT16* pattern, UINT PatternSz, int RetOffset, bool deref)
{
	static HMODULE hModule = GetModuleHandle(L"ac_client.exe");
	static MODULEINFO ModuleInfo{ NULL };

	if (!ModuleInfo.lpBaseOfDll) GetModuleInformation(GetCurrentProcess(), hModule, &ModuleInfo, sizeof(MODULEINFO));

	BYTE* data = static_cast<BYTE*>(ModuleInfo.lpBaseOfDll);
	const BYTE* ScanEnd = (data + ModuleInfo.SizeOfImage) - PatternSz;

	while (data < ScanEnd)
	{
		UINT MatchingBytes = 0;

		for (UINT i = 0; i < PatternSz; ++i, ++data, ++MatchingBytes)
		{
			if (pattern[i] == unk) continue;
			if (static_cast<BYTE>(pattern[i]) != *data) break;
		}

		if (MatchingBytes == PatternSz)
		{
			BYTE* const ResolvedAddress = (data - MatchingBytes) + RetOffset;
			return deref ? *reinterpret_cast<BYTE**>(ResolvedAddress) : ResolvedAddress;
		}

		++data;
	}

	return nullptr; 
}