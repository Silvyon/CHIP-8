#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <Windows.h>
#include <conio.h>
#include "chip8.h"
#include <SDL3/SDL.h>

SDL_Window* window;
SDL_Renderer* renderer;

void setUpGraphics()
{
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("CHIP-8", 640, 320, 0);
	renderer = SDL_CreateRenderer(window, NULL);
	SDL_SetRenderLogicalPresentation(renderer, 64, 32, SDL_LOGICAL_PRESENTATION_STRETCH);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void drawGraphics()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			gfx[y * SCREEN_WIDTH + x] = 1;
			if (gfx[y * SCREEN_WIDTH + x])
			{
				SDL_RenderPoint(renderer, x, y);
			}
		}
	}
}
