#include "pch.hpp"
#include "gui.hpp"
#include "init.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(lpReserved);

    if (fdwReason == DLL_PROCESS_ATTACH)
    {
    #ifdef _DEBUG

        AllocConsole();
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);

    #endif

        SDL_Window* window = InitGui();
        if (!window)
        {
            MessageBox(nullptr, L"GUI initialization failed", L"ERROR", MB_ICONERROR | MB_OK);
            return false;
        }

        if (!InitCheat(window))
        {
            MessageBox(nullptr, L"Cheat initialization failed", L"ERROR", MB_ICONERROR | MB_OK);
            return false;
        }

        DBG_OUT("\nInjection successful!\n");
    }
    
    return true;
}