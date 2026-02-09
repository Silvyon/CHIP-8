#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <Windows.h>
#include <conio.h>
#include "chip8.h"
#include <SDL3/SDL.h>



int main(int argc, const char* argv[])
{
	setUpGraphics();
	setUpInput();

	initialize();
	loadGame("petdog.ch8");

	int running = 1;
	while(running)
	{
		emulateCycle();

		uint32_t now = SDL_GetTicks();
		if(now - lastTimerUpdate >= MS_PER_TIMER_CYCLE)
		{
			updateTimers();
			lastTimerUpdate = now;
		}

		if(drawFlag)
		{
			drawGraphics();
		}

		setKeys();

		SDL_Delay(MS_PER_CPU_CYCLE);
	}

	return 0;
}
