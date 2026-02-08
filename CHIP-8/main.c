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
	loadGame("C:/Users/Adi Silverman/source/octojam1title.ch8");

	int running = 1;
	while(running)
	{
		emulateCycle();

		if(drawFlag)
		{
			drawGraphics();
		}

		setKeys();
	}

	return 0;
}
