#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <Windows.h>
#include <conio.h>
#include "chip8.h"

void OP_00E0() // Clears the screen
{
	for (int y = 0; y < SCREEN_HEIGHT; y++)
		for (int x = 0; x < SCREEN_WIDTH; x++)
			gfx[y * SCREEN_WIDTH + x] = 0;
}

void OP_00EE() // Returns from a subroutine
{
	sp--;
	pc = stack[sp];
}

void OP_1NNN() // Jumps to address NNN
{
	pc = opcode & 0x0FFF;
}

void OP_2NNN() // Calls subroutine at NNN
{
	stack[sp] = pc;
	++sp;
	pc = opcode & 0x0FFF;
}

void OP_3XNN() // Skips the next instruction if VX equals NN
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t NN = opcode & 0x00FF;
	if (reg[X] == NN)
		pc += 2;
}

void OP_4XNN() // Skips the next instruction if VX does not equal NN
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t NN = opcode & 0x00FF;
	if (reg[X] != NN)
		pc += 2;
}

void OP_5XY0() // Skips the next instruction if VX equals VY
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t Y = (opcode & 0x00F0) >> 4;
	if (reg[X] == reg[Y])
		pc += 2;
}

void OP_6XNN() // Sets VX to NN
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t NN = opcode & 0x00FF;
	reg[X] = NN;
}

void OP_7XNN() // Adds NN to VX
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t NN = opcode & 0x00FF;
	reg[X] += NN;
}

void OP_8XY0() // Sets VX to the value of VY
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t Y = (opcode & 0x00F0) >> 4;
	reg[X] = reg[Y];
}

void OP_8XY1() // Sets VX to VX or VY
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t Y = (opcode & 0x00F0) >> 4;
	reg[X] |= reg[Y];
}

void OP_8XY2() // Sets VX to VX and VY
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t Y = (opcode & 0x00F0) >> 4;
	reg[X] &= reg[Y];
}

void OP_8XY3() // Sets VX to VX xor VY
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t Y = (opcode & 0x00F0) >> 4;
	reg[X] ^= reg[Y];
}

void OP_8XY4() // Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t Y = (opcode & 0x00F0) >> 4;
	reg[VF] = (reg[X] < 256 - reg[Y]);
	reg[X] += reg[Y];
}

void OP_8XY5() // VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not.
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t Y = (opcode & 0x00F0) >> 4;
	reg[VF] = (reg[X] >= reg[Y]);
	reg[X] -= reg[Y];
}

void OP_8XY6() // Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	reg[VF] = reg[X] & 0x0001;
	reg[X] >>= 1;
}

void OP_8XY7() // Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not.
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t Y = (opcode & 0x00F0) >> 4;
	reg[VF] = (reg[Y] >= reg[X]);
	reg[X] = reg[Y] - reg[X];
}

void OP_8XYE() // Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	reg[VF] = (reg[X] & 0x0080) >> 7;
	reg[X] <<= 1;                     
}

void OP_9XY0() // Skips the next instruction if VX does not equal VY
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t Y = (opcode & 0x00F0) >> 4;
	if (reg[X] != reg[Y])
		pc += 2;
}

void OP_ANNN() // Sets I to the address NNN
{
	I = opcode & 0x0FFF;
}

void OP_BNNN() // Jumps to the address NNN plus V0
{
	uint16_t NNN = opcode & 0x0FFF;
	pc = NNN + reg[V0];
}

void OP_CXNN() // Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t NN = opcode & 0x00FF;
	reg[X] = (rand() & 0xFF) & NN;
}

void OP_DXYN() // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t Y = (opcode & 0x00F0) >> 4;
	uint8_t N = opcode & 0x000F;

	uint8_t xPos = reg[X];
	uint8_t yPos = reg[Y];

	reg[VF] = 0;

	for (int row = 0; row < N; row++)
	{
		uint8_t spriteByte = memory[I + row];

		for (int col = 0; col < 8; col++)
		{
			uint8_t spritePixel = (spriteByte >> (7 - col)) & 1;

			int x = (xPos + col) % 64;
			int y = (yPos + row) % 32;

			int index = y * 64 + x;

			if (spritePixel)
			{
				if (gfx[index] == 1)
					reg[VF] = 1;

				gfx[index] ^= 1;
			}
		}
	}
	drawFlag = 1;
}

void OP_EX9E() // Skips the next instruction if the key stored in VX(only consider the lowest nibble) is pressed
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t key = reg[X] & 0x0F;

	if (keypad[key])
		pc += 2;
}

void OP_EXA1() // Skips the next instruction if the key stored in VX(only consider the lowest nibble) is not pressed
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t key = reg[X] & 0x0F;

	if (!keypad[key])
		pc += 2;
}

void OP_FX07() // Sets VX to the value of the delay timer
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	reg[X] = delay_timer;
}

void OP_FX0A() // A key press is awaited, and then stored in VX
{
	uint8_t X = (opcode & 0x0F00) >> 8;

	for (uint8_t key = 0; key < 16; key++)
	{
		if (keypad[key])
		{
			reg[X] = key;
			waitingForKeyFlag = 0;
			return;
		}
	}
	waitingForKeyFlag = 1;
}

void OP_FX15() // Sets the delay timer to VX
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	delay_timer = reg[X];
}

void OP_FX18() // Sets the sound timer to VX
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	sound_timer = reg[X];
}

void OP_FX1E() // Adds VX to I. VF is not affected
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	I += reg[X];
}

void OP_FX29() // Sets I to the location of the sprite for the character in VX(only consider the lowest nibble). Characters 0-F (in hexadecimal) are represented by a 4x5 font
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t character = reg[X] & 0x0F;

	I = character * 5;
}

void OP_FX33() // Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t value = reg[X];
	memory[I] = value / 100;
	memory[I + 1] = (value / 10) % 10;
	memory[I + 2] = value % 10;
}

void OP_FX55() // Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	for(int i = 0; i <= X; i++)
	{
		memory[I + i] = reg[i];
	}
}

void OP_FX65() // Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified
{
	uint8_t X = (opcode & 0x0F00) >> 8;
	for (int i = 0; i <= X; i++)
	{
		reg[i] = memory[I + i];
	}
}