int HandleEvent(SDL_Event* sdl_event);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef LRESULT(CALLBACK* WndProcSig)(HWND, UINT, WPARAM, LPARAM);

typedef int(*PollEventsSig)(SDL_Event*);