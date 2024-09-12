int HandleEvent(SDL_Event* sdl_event);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef LRESULT(CALLBACK* WndProc_sig)(HWND, UINT, WPARAM, LPARAM);

typedef int(*PollEvents_sig)(SDL_Event*);