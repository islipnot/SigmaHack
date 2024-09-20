#pragma warning(disable : 6387)

#include "pch.h"
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

void __VirtualProtect(void* address, int sz)
{
	DWORD old;
	VirtualProtect(address, sz, PAGE_EXECUTE_READWRITE, &old);
}

void SetHook(BYTE* HookAddr, const BYTE* dst, int ExtraBytes = 0)
{
	__VirtualProtect(HookAddr, 5 + ExtraBytes);

	*HookAddr = 0xE8; // 0xE8 = call
	*reinterpret_cast<DWORD*>(HookAddr + 1) = dst - (HookAddr + 5);

	if (ExtraBytes) memset(HookAddr + 5, 0x90, ExtraBytes); // NOP'ing specified bytes
}

void ResolvePatterns()
{
	// Defining patterns

	constexpr BYTE EntListPattern[] = {
		0xA1, unk, unk, unk, unk,                   // mov eax, ent_list
		0x8B, 0x34, 0xB8,                           // mov esi, [eax+edi*4]
		0x85, 0xF6,                                 // test esi, esi
		0x0F, 0x84,                                 // jz x
	};
	constexpr BYTE FovPattern[] = {
		0xF3, 0x0F, 0x10, 0x05, unk, unk, unk, unk, // movss xmm0, fov
		0x5B,                                       // pop ebx
		0xC3                                        // retn
	};
	constexpr BYTE RollPattern[] = {
		0xF3, 0x0F, 0x11, 0x56, 0x3C,               // movss [esi+0xROLL_OFFSET], xmm0
		0x3B, 0x35, unk, unk, unk, unk,             // cmp esi, local_player
		0x74                                        // jz x
	};
	constexpr BYTE SpreadPattern[] = {
		0xFF, 0x50, 0x04,                           // call [eax+4]
		0x8B, 0x17,                                 // mov edx, [edi]
		0x8B, 0xF0                                  // mov esi, eax
	};
	constexpr BYTE vRecoilPattern[] = {
		0xF3, 0x0F, 0x59, 0x05, unk, unk, unk, unk, // mulss xmm0, x
		0xF3, 0x0F, 0x5D, 0xD0                      // minss xmm2, xmm0
	};

	// Getting entity pointers

	const auto ent_list_ptr = reinterpret_cast<player_entity***>(ResolveAddress(EntListPattern, sizeof(EntListPattern), 1));

	pPlayerList = ent_list_ptr;
	PlayerCount    = reinterpret_cast<UINT*>(ent_list_ptr + 2);
	LocalPlayer    = *reinterpret_cast<player_entity**>(ent_list_ptr - 1); 

	// Getting FOV pointer

	cfg.pFov = reinterpret_cast<float*>(ResolveAddress(FovPattern, sizeof(FovPattern), 4));
	cfg.fov = *cfg.pFov;

	// Getting roll patch address

	BYTE* RollPatch = ResolveAddress(RollPattern, sizeof(RollPattern), 0, false);

	__VirtualProtect(RollPatch, 5);
	memset(RollPatch, 0x90, 5);

	// Hooking the address at which spread is determined

	BYTE* SpreadAddr = ResolveAddress(SpreadPattern, sizeof(SpreadPattern), 0, false);
	SetHook(SpreadAddr, reinterpret_cast<BYTE*>(SpreadDispatch), 0);

	// Locating the address at which the visual recoil multiplier is to be copied

	BYTE* vRecoilAddr = ResolveAddress(vRecoilPattern, sizeof(vRecoilPattern), 4, false);
	__VirtualProtect(vRecoilAddr, 4);

	float* pVisRecoil = &cfg.VisRecoilMulti;
	memcpy(vRecoilAddr, &pVisRecoil, sizeof(float*));
}

void* HookExport(HMODULE hModule, const char* fnName, DWORD ExportHook)
{
	auto target_export = *reinterpret_cast<DWORD**>((reinterpret_cast<BYTE*>(GetProcAddress(hModule, fnName)) + 2));
	auto old_func_addr = *reinterpret_cast<DWORD**>(target_export);

	__VirtualProtect(target_export, sizeof(DWORD));

	*target_export = ExportHook;
	return old_func_addr;
}

void initilization(SDL_Window* window)
{
	// Setting hooks on export table(s)
	const HMODULE hModule = GetModuleHandle(L"sdl2.dll");
	SwapWindow = static_cast<SwapWindowSig>(HookExport(hModule, "SDL_GL_SwapWindow", reinterpret_cast<DWORD>(DrawMenu)));
	PollEvents = static_cast<PollEventsSig>(HookExport(hModule, "SDL_PollEvent", reinterpret_cast<DWORD>(HandleEvent)));
	
	// Hooking the WndProc
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	ogWndProc = reinterpret_cast<WndProcSig>(SetWindowLongW(wmInfo.info.win.window, GWLP_WNDPROC, reinterpret_cast<long>(WndProc)));

	// Resolving addresses in memory via pattern scanning
	ResolvePatterns();
}