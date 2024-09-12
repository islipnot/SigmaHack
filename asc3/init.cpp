#pragma warning(disable : 6387)

#include "pch.h"
#include "gui.hpp"
#include "init.hpp"
#include "input.hpp"
#include "weapon.hpp"
#include "entities.hpp"
#include "pScanning.hpp"

extern WndProc_sig og_wndproc;
extern PollEvents_sig PollEvents;
extern SwapWindow_sig SwapWindow;

static void __VirtualProtect(void* const address, const int sz)
{
	DWORD old;
	VirtualProtect(address, sz, PAGE_EXECUTE_READWRITE, &old);
}

static void SetHook(BYTE* const hook_addr, const BYTE* const dst, const int extra_bytes = 0)
{
	__VirtualProtect(hook_addr, 5 + extra_bytes);

	*hook_addr = 0xE8; // 0xE8 = call
	*reinterpret_cast<DWORD*>(hook_addr + 1) = dst - (hook_addr + 5);

	if (extra_bytes) memset(hook_addr + 5, 0x90, extra_bytes); // NOP'ing specified bytes
}

static void ResolvePatterns()
{
	// Getting entity pointers
	constexpr BYTE ent_list_pattern[] = {
		0xA1, unk, unk, unk, unk, // mov eax, ent_list
		0x8B, 0x34, 0xB8,         // mov esi, [eax+edi*4]
		0x85, 0xF6,               // test esi, esi
		0x0F, 0x84,               // jz x
	};
	const auto ent_list_ptr = reinterpret_cast<player_entity***>(ResolveAddress(ent_list_pattern, sizeof(ent_list_pattern), 1));

	player_list_ptr = ent_list_ptr;
	player_count = reinterpret_cast<UINT*>(ent_list_ptr + 2);
	local_player = *reinterpret_cast<player_entity**>(ent_list_ptr - 1); 

	// Getting FOV pointer
	constexpr BYTE fov_pattern[] = {
		0xF3, 0x0F, 0x10, 0x05, unk, unk, unk, unk, // movss xmm0, fov
		0x5B,                                       // pop ebx
		0xC3                                        // retn
	};
	cfg.pFov = reinterpret_cast<float*>(ResolveAddress(fov_pattern, sizeof(fov_pattern), 4));
	cfg.fov = *cfg.pFov;

	// Getting address where roll is set
	constexpr BYTE roll_pattern[] = {
		0xF3, 0x0F, 0x11, 0x56, 0x3C,   // movss [esi+0xROLL_OFFSET], xmm0
		0x3B, 0x35, unk, unk, unk, unk, // cmp esi, local_player
		0x74                            // jz x
	};
	BYTE* const roll_patch = ResolveAddress(roll_pattern, sizeof(roll_pattern), 0, false);

	__VirtualProtect(roll_patch, 5);
	memset(roll_patch, 0x90, 5);

	// Getting address of the call to vFunc responsible for getting a spread value
	constexpr BYTE spread_pattern[] = {
		0xFF, 0x50, 0x04, // call [eax+4]
		0x8B, 0x17,       // mov edx, [edi]
		0x8B, 0xF0        // mov esi, eax
	};
	BYTE* const spread_addr = ResolveAddress(spread_pattern, sizeof(spread_pattern), 0, false);

	SetHook(spread_addr, reinterpret_cast<BYTE*>(SpreadDispatch), 0);

	// Locating address at which the visual recoil multiplier is to be applied
	constexpr BYTE vrecoil_pattern[] = {
		0xF3, 0x0F, 0x59, 0x05, unk, unk, unk, unk, // mulss xmm0, x
		0xF3, 0x0F, 0x5D, 0xD0                      // minss xmm2, xmm0
	};
	BYTE* const vrecoil_addr = ResolveAddress(vrecoil_pattern, sizeof(vrecoil_pattern), 4, false);

	__VirtualProtect(vrecoil_addr, 4);
	const float* const vis_recoil_ptr = &cfg.vis_recoil_mlt;
	memcpy(vrecoil_addr, &vis_recoil_ptr, sizeof(float*));
}

static void* HookExport(const HMODULE hModule, const char* func_name, const DWORD export_replacement)
{
	auto target_export = *reinterpret_cast<DWORD**>((reinterpret_cast<BYTE*>(GetProcAddress(hModule, func_name)) + 2));
	auto old_func_addr = *reinterpret_cast<DWORD**>(target_export);

	__VirtualProtect(target_export, sizeof(DWORD));

	*target_export = export_replacement;
	return old_func_addr;
}

void initilization(SDL_Window* window)
{
	// Setting hooks on export table(s)
	const HMODULE hModule = GetModuleHandle(L"sdl2.dll");
	SwapWindow = static_cast<SwapWindow_sig>(HookExport(hModule, "SDL_GL_SwapWindow", reinterpret_cast<DWORD>(DrawMenu)));
	PollEvents = static_cast<PollEvents_sig>(HookExport(hModule, "SDL_PollEvent", reinterpret_cast<DWORD>(HandleEvent)));
	
	// Hooking the WndProc
	SDL_SysWMinfo wm_info;
	SDL_VERSION(&wm_info.version);
	SDL_GetWindowWMInfo(window, &wm_info);
	og_wndproc = reinterpret_cast<WndProc_sig>(SetWindowLongW(wm_info.info.win.window, GWLP_WNDPROC, reinterpret_cast<long>(WndProc)));

	// Resolving addresses in memory via pattern scanning
	ResolvePatterns();
}