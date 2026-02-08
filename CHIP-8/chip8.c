#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <Windows.h>
#include <conio.h>
#include "chip8.h"

//memory
uint8_t memory[MEMORY_MAX];

//register storage
uint8_t reg[16];


uint16_t I; // index register
uint16_t pc; // progaram counter

//flasg
uint16_t drawFlag;
uint16_t waitingForKeyFlag;

//display
uint8_t gfx[SCREEN_WIDTH * SCREEN_HEIGHT];

//timers
uint8_t delay_timer;
uint8_t sound_timer;

//stack
uint16_t stack[16];
uint16_t sp; //stack pointer

//keypad
uint8_t keypad[16];

//opcode
uint16_t opcode;

//font set
uint8_t chip8_fontset[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void initialize()
{
	pc = 0x200;
	opcode = 0;
	I = 0;
	sp = 0;

	//clear display
	for (int r = 0; r < SCREEN_WIDTH; r++)
		for(int c = 0; c < SCREEN_HEIGHT; c++)
			gfx[r * SCREEN_WIDTH + c] = 0;

	//clear stack
	for (int i = 0; i < 16; i++)
		stack[i] = 0;

	//clear regiesters V0 - VF
	for (int i = 0; i < VF; i++)
		reg[i] = 0;

	//clear memory
	for (int i = 0; i < MEMORY_MAX; i++)
		memory[i] = 0;

	//load fontset
	for (int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];

	//reset timers
	delay_timer = 60;
	sound_timer = 60;

	//change rand seed
	srand(time(NULL));

	//reset flags
	drawFlag = 0;
	waitingForKeyFlag = 0;
}

void loadGame(const char* gameName)
{
	FILE* game = fopen(gameName, "rb");
	if (game == NULL) {
		printf("Failed to open %s\n", gameName);
		return;
	}
	fread(&memory[PROGRAM_START], 1, MEMORY_MAX - PROGRAM_START, game);
	fclose(game);
}

void emulateCycle()
{
	//fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	//decode opcode
	switch (opcode & 0xF000)
	{

	case 0x0000:
		switch (opcode & 0x000F)
		{

		case 0x0000: // 0x00E0: Clears the screen        
			OP_00E0();
			break;

		case 0x000E: // 0x00EE: Returns from subroutine          
			OP_00EE();
			break;

		default: //unknown opcode
			printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
		}

		break;

	case 0x2000: // 0x2NNN: calls subroutine at NNN
		OP_2NNN();
		break;

	case 0x3000: // 0x3XNN: Skips the next instruction if VX equals NN 
		OP_3XNN();
		break;

	case 0x4000: // 0x4XNN: Skips the next instruction if VX does not equal NN
		OP_4XNN();
		break;

	case 0x5000: // 0x5XY0: Skips the next instruction if VX equals VY
		OP_5XY0();
		break;

	case 0x6000: // 0x6XNN: Sets VX to NN
		OP_6XNN();
		break;

	case 0x7000: // 0x7Xnn: Adds NN to VX
		OP_7XNN();
		break;

	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000: // 0x8XY0: Sets VX to the value of VY
			OP_8XY0();
			break;

		case 0x0001: // 0x8XY1: Sets VX to VX or VY
			OP_8XY1();
			break;

		case 0x0002: // 0x8XY2: Sets VX to VX and VY
			OP_8XY2();
			break;

		case 0x0003: // 0x8XY3: Sets VX to VX xor VY
			OP_8XY3();
			break;

		case 0x0004: // 0x8XY4: Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not
			OP_8XY4();
			break;

		case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not
			OP_8XY5();
			break;

		case 0x0006: // 0x8XY6: Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF
			OP_8XY6();
			break;

		case 0x0007: // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not
			OP_8XY7();
			break;

		case 0x000E: // 0x8XYE: Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset
			OP_8XYE();
			break;

		default: //unknown opcode
			printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
		}
		break;	

	case 0x9000: // 9XY0: Skips the next instruction if VX does not equal VY
		OP_9XY0();
		break;

	case 0xA000: // 0xANNN: Sets I to the address NNN
		OP_ANNN();
		break;

	case 0xB000: // 0xBNNN: Jumps to the address NNN plus V0
		OP_BNNN();
		break;

	case 0xC000: // 0xCXNN: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN
		OP_CXNN();
		break;

	case 0xD000: // 0xDXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen
		OP_DXYN();
		break;

	case 0xE000:
		switch (opcode & 0x000F)
		{
		case 0x000E: // 0xEX9E: Skips the next instruction if the key stored in VX(only consider the lowest nibble) is pressed
			OP_EX9E();
			break;

		case 0x0001: // 0xEXA1: Skips the next instruction if the key stored in VX(only consider the lowest nibble) is not pressed
			OP_EXA1();
			break;
		}

	case 0xF000:
		switch (opcode & 0x00FF)
		{
		case 0x0007: // Sets VX to the value of the delay timer
			OP_FX07();
			break;

		case 0x000A: // A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event, delay and sound timers should continue processing)
			OP_FX0A();
			break;

		case 0x0015: // Sets the delay timer to VX
			OP_FX15();
			break;

		case 0x0018: // Sets the sound timer to VX
			OP_FX18();
			break;

		case 0x001E: // Adds VX to I. VF is not affected
			OP_FX1E();
			break;

		case 0x0029: // Sets I to the location of the sprite for the character in VX(only consider the lowest nibble). Characters 0-F (in hexadecimal) are represented by a 4x5 font
			OP_FX29();
			break;

		case 0x0033: // Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2
			OP_FX33();
			break;

		case 0x0055: // Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified
			OP_FX55();
			break;

		case 0x0065: // Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified
			OP_FX65();
			break;

		default: //unknown opcode
			printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
		}

	default: //unknown opcode
		printf("Unknown opcode: 0x%X\n", opcode);
	}

	//update timers
	if(delay_timer > 0)
	{
		--delay_timer;
	}

	if(sound_timer > 0)
	{
		if(sound_timer == 1)
		{
			printf("BEEP! \n");
		}
		--sound_timer;
	}

	//increment pc
	if (!waitingForKeyFlag)
	{
		pc += 2;
	}
}


