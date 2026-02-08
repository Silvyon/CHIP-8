#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <Windows.h>
#include <conio.h>
#include "chip8.h"

void main(int argc, const char* argv[])
{
	//setUpGraphics();
	//setUpInput();

	initialize();

	int running = 1;
	while(running)
	{
		emulateCycle();

		if(drawFlag)
		{
			//drawGraphics();
		}
	}
}
