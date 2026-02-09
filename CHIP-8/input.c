#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <Windows.h>
#include <conio.h>
#include "chip8.h"
#include <SDL3/SDL.h>

void setUpInput()
{

}

void setKeys()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return;
        }
        else if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
            bool isDown = event.type == SDL_EVENT_KEY_DOWN;
            switch (event.key.key) {
            case SDLK_1: keypad[0] = isDown; break;
            case SDLK_2: keypad[1] = isDown; break;
            case SDLK_3: keypad[2] = isDown; break;
            case SDLK_4: keypad[3] = isDown; break;
            case SDLK_Q: keypad[4] = isDown; break;
            case SDLK_W: keypad[5] = isDown; break;
            case SDLK_E: keypad[6] = isDown; break;
            case SDLK_R: keypad[7] = isDown; break;
            case SDLK_A: keypad[8] = isDown; break;
            case SDLK_S: keypad[9] = isDown; break;
            case SDLK_D: keypad[10] = isDown; break;
            case SDLK_F: keypad[11] = isDown; break;
            case SDLK_Z: keypad[12] = isDown; break;
            case SDLK_X: keypad[13] = isDown; break;
            case SDLK_C: keypad[14] = isDown; break;
            case SDLK_V: keypad[15] = isDown; break;
            }
        }
    }

}