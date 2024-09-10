#include "pch.h"
#include "gui.hpp"
#include "init.hpp"

static BOOL APIENTRY DllMain(HMODULE hModule, DWORD call_reason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(lpReserved);

    if (call_reason == DLL_PROCESS_ATTACH)
    {
        AllocConsole();
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        SDL_Window* window = InitGui();
        initilization(window);
    }

    return TRUE;
}