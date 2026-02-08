#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define MEMORY_MAX 4096
#define PROGRAM_START 0x200

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

//memory
extern uint8_t memory[MEMORY_MAX];


//regiester
enum
{
	V0 = 0,
	V1,
	V2,
	V3,
	V4,
	V5,
	V6,
	V7,
	V8,
	V9,
	V10,
	V11,
	V12,
	V13,
	V14,
	VF
};

//register storage
extern uint8_t reg[16];


extern uint16_t I; // index register
extern uint16_t pc; // progaram counter

//flags
extern uint16_t drawFlag;
extern uint16_t waitingForKeyFlag;

//display
extern uint8_t gfx[SCREEN_WIDTH * SCREEN_HEIGHT];

//timers
extern uint8_t delay_timer;
extern uint8_t sound_timer;

//stack
extern uint16_t stack[16];
extern uint16_t sp; //stack pointer

//keypad
extern uint8_t keypad[16];

//opcode
extern uint16_t opcode;

//font set
extern uint8_t chip8_fontset[80];


void initialize();

void loadGame(const char *gameName);

void emulateCycle();

//opcode functions
void OP_00E0();

void OP_00EE();

void OP_1NNN();

void OP_2NNN();

void OP_3XNN();;

void OP_4XNN();

void OP_5XY0();

void OP_6XNN();

void OP_7XNN();

void OP_8XY0();

void OP_8XY1();

void OP_8XY2();

void OP_8XY3();

void OP_8XY4();

void OP_8XY5();

void OP_8XY6();

void OP_8XY7();

void OP_8XYE();

void OP_9XY0();

void OP_ANNN();

void OP_BNNN();

void OP_CXNN();

void OP_DXYN();

void OP_EX9E();

void OP_EXA1();

void OP_FX07();

void OP_FX0A();

void OP_FX15();

void OP_FX18();

void OP_FX1E();

void OP_FX29();

void OP_FX33();

void OP_FX55();

void OP_FX65();

#endif