#pragma once

typedef void(*SwapWindowSig)(SDL_Window*);

SDL_Window* InitGui();

void DrawMenu(SDL_Window* window);