#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <Windows.h>
#include <conio.h>
#include "chip8.h"
#include <SDL3/SDL.h>

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;

void setUpGraphics()
{
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("CHIP-8", 640, 320, 0);
	renderer = SDL_CreateRenderer(window, NULL);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}

void drawGraphics()
{
	void* pixels;
	int pitch;
	SDL_LockTexture(texture, NULL, &pixels, &pitch);

	SDL_UnlockTexture(texture);

	uint32_t* screen = (uint32_t*)pixels;

	for (int i = 0; i < 64 * 32; i++)
	{
		if (gfx[i] == 0)
			screen[i] = 0x000000FF;
		else
			screen[i] = 0xFFFFFFFF;
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // Black background
	SDL_RenderClear(renderer);

	SDL_RenderTexture(renderer, texture, NULL, NULL);

	SDL_RenderPresent(renderer);

	drawFlag = 0;
}
