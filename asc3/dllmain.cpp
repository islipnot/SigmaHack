#include "pch.h"
#include "gui.hpp"
#include "init.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD call_reason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(lpReserved);

    if (call_reason == DLL_PROCESS_ATTACH)
    {
    #ifdef _DEBUG
        AllocConsole();
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
    #endif

        SDL_Window* window = InitGui();
        initilization(window);
    }

    return TRUE;
}