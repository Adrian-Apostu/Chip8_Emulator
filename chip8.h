#ifndef CHIP8_EMULATOR_CHIP8_H
#define CHIP8_EMULATOR_CHIP8_H
#include <stdint.h>
#include <stdbool.h>

typedef struct Chip8 {
    uint8_t memory[4096]; //Memory
    uint8_t display[64 * 32]; //Display Size width * height
    uint16_t stack[16]; //Stack
    uint8_t V[16]; //Registers
    uint8_t keypad[16]; //Input
    uint16_t I; //Index Register
    uint16_t pc; //Program Counter
    uint8_t sp; //Stack Pointer
    uint8_t delay_timer; //Timer
    uint8_t sound_timer; //Timer
    bool draw_flag; //Flags
} chip8_t;

void chip8_init(chip8_t *chip8);
bool chip8_load_rom(chip8_t *chip8, const char *filename);
void chip8_cycle(chip8_t *chip8);

#endif