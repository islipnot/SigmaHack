#pragma warning(disable : 6387)

#include "pch.hpp"
#include "gui.hpp"
#include "init.hpp"
#include "input.hpp"
#include "weapon.hpp"
#include "config.hpp"
#include "entities.hpp"
#include "pScanning.hpp"

extern WndProcSig ogWndProc;
extern PollEventsSig PollEvents;
extern SwapWindowSig SwapWindow;

BOOL VirtualProtectERW(void* address, int sz)
{
	DWORD old;
	return VirtualProtect(address, sz, PAGE_EXECUTE_READWRITE, &old);
}

bool SetHook(BYTE* pHook, const BYTE* dst, int ExtraBytes = 0)
{
	if (!VirtualProtectERW(pHook, 5 + ExtraBytes))
	{
		EDBG_OUT("Failed to set hook protection (" << GetLastError() << ")\n");
		return false;
	}
	DBG_ELSE_OUT("pHook protection set\n");

	*pHook = 0xE8; // 0xE8 = call
	*reinterpret_cast<DWORD*>(pHook + 1) = dst - (pHook + 5);
	
	if (ExtraBytes) memset(pHook + 5, 0x90, ExtraBytes); // NOP'ing specified bytes
	return true;
}

bool ResolvePatterns()
{
	// Defining patterns

	constexpr UINT16 EntListPattern[] = 
	{
		0xA1, unk, unk, unk, unk,                   // mov eax, ent_list
		0x8B, 0x34, 0xB8,                           // mov esi, [eax+edi*4]
		0x85, 0xF6,                                 // test esi, esi
		0x0F, 0x84,                                 // jz x
	};
	constexpr UINT16 FovPattern[] = 
	{
		0xF3, 0x0F, 0x10, 0x05, unk, unk, unk, unk, // movss xmm0, fov
		0x5B,                                       // pop ebx
		0xC3                                        // retn
	};
	constexpr UINT16 RollPattern[] = 
	{
		0xF3, 0x0F, 0x11, 0x56, 0x3C,               // movss [esi+0xROLL_OFFSET], xmm0
		0x3B, 0x35, unk, unk, unk, unk,             // cmp esi, local_player
		0x74                                        // jz x
	};
	constexpr UINT16 SpreadPattern[] = 
	{
		0xFF, 0x50, 0x04,                           // call [eax+4]
		0x8B, 0x17,                                 // mov edx, [edi]
		0x8B, 0xF0                                  // mov esi, eax
	};
	constexpr UINT16 vRecoilPattern[] = 
	{
		0xF3, 0x0F, 0x59, 0x05, unk, unk, unk, unk, // mulss xmm0, x
		0xF3, 0x0F, 0x5D, 0xD0                      // minss xmm2, xmm0
	};

	// Getting entity pointers

	const auto pEntList = ResolveAddressEx<PlayerEnt***>(EntListPattern, PtrnSz(EntListPattern), 1);

	if (!pEntList)
	{
		EDBG_OUT("Failed to resolve EntListPattern\n");
		return false;
	}
	DBG_ELSE_OUT("Resolved EntListPattern\n");

	pPlayerList = pEntList;
	PlayerCount = reinterpret_cast<UINT*>(pEntList + 2);
	LocalPlayer = *reinterpret_cast<PlayerEnt**>(pEntList - 1); 

	// Getting FOV pointer

	cfg.pFov = ResolveAddressEx<float*>(FovPattern, PtrnSz(FovPattern), 4);

	if (!cfg.pFov)
	{
		EDBG_OUT("Failed to resolve FovPattern\n");
		return false;
	}
	DBG_ELSE_OUT("Resolved FovPattern\n");

	cfg.fov = *cfg.pFov;

	// Getting roll patch address

	BYTE* const pRoll = ResolveAddress(RollPattern, PtrnSz(RollPattern));

	if (!pRoll)
	{
		EDBG_OUT("Failed to resolve RollPattern\n");
		return false;
	}
	DBG_ELSE_OUT("Resolved RollPattern\n");

	VirtualProtectERW(pRoll, 5);
	memset(pRoll, 0x90, 5);

	// Hooking the address at which spread is determined

	BYTE* const pSpread = ResolveAddress(SpreadPattern, PtrnSz(SpreadPattern));

	if (!pSpread)
	{
		EDBG_OUT("Failed to resolve SpreadPattern\n");
		return false;
	}
	DBG_ELSE_OUT("Resolved SpreadPattern\n");

	SetHook(pSpread, reinterpret_cast<BYTE*>(SpreadDispatch), 0);

	// Locating the address at which the visual recoil multiplier is to be copied
	
	BYTE* const pVisRecoilPatch = ResolveAddress(vRecoilPattern, PtrnSz(vRecoilPattern), 4);

	if (!pVisRecoilPatch)
	{
		EDBG_OUT("Failed to resolve vRecoilPattern\n");
		return false;
	}
	DBG_ELSE_OUT("Resolved vRecoilPattern\n");

	VirtualProtectERW(pVisRecoilPatch, 4);

	float* pVisRecoil = &cfg.VisRecoilMulti;
	memcpy(pVisRecoil, &pVisRecoil, sizeof(float*));

	return true;
}

void* HookExport(HMODULE hModule, const char* fnName, DWORD ExportHook)
{
	DWORD* TargetExport = *reinterpret_cast<DWORD**>((reinterpret_cast<BYTE*>(GetProcAddress(hModule, fnName)) + 2));
	DWORD* pOldFunc = *reinterpret_cast<DWORD**>(TargetExport);

	if (!VirtualProtectERW(TargetExport, sizeof(DWORD*)))
	{
		EDBG_OUT("Failed to set protection on TargetExport! (" << GetLastError << ")\n");
		return nullptr;
	}
	DBG_ELSE_OUT("TargetExport protection set!\n");

	*TargetExport = ExportHook;
	return pOldFunc;
}

bool InitCheat(SDL_Window* const window)
{
	// Setting hooks on export table(s)
	const HMODULE hModule = GetModuleHandle(L"sdl2.dll");

	SwapWindow = static_cast<SwapWindowSig>(HookExport(hModule, "SDL_GL_SwapWindow", reinterpret_cast<DWORD>(DrawMenu)));

	if (!static_cast<void*>(SwapWindow))
	{
		EDBG_OUT("Failed to hook SDL_GL_SwapWindow\n");
		return false;
	}
	DBG_ELSE_OUT("Hooked SDL_GL_SwapWindow\n");

	PollEvents = static_cast<PollEventsSig>(HookExport(hModule, "SDL_PollEvent", reinterpret_cast<DWORD>(HandleEvent)));

	if (!static_cast<void*>(PollEvents))
	{
		EDBG_OUT("Failed to hook SDL_PollEvent\n");
		return false;
	}
	DBG_ELSE_OUT("Hooked SDL_PollEvent\n");
	
	// Hooking the WndProc
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	ogWndProc = reinterpret_cast<WndProcSig>(SetWindowLongW(wmInfo.info.win.window, GWLP_WNDPROC, reinterpret_cast<long>(WndProc)));

	// Resolving addresses in memory via pattern scanning
	return ResolvePatterns();
}