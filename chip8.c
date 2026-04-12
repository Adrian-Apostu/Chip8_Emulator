#include "chip8.h"
#include <stdio.h>
#include <string.h>

static const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
    0x20, 0x60, 0x20, 0x20, 0x70,		// 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
    0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
    0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
    0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
    0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
    0xF0, 0x80, 0xF0, 0x80, 0x80		// F
};

void chip8_init(chip8_t *chip8) {
    memset(chip8, 0, sizeof(chip8_t));
    chip8->pc = 0x200;
    memcpy(&chip8->memory[0x50], fontset, 80);
}

bool chip8_load_rom(chip8_t *chip8, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error! Could not open the file\n");
        return false;
    }

    fseek(file,0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (file_size > (4096 - 0x200)) {
        printf("The file is too large\n");
        fclose(file);
        return false;
    }

    fread(&chip8->memory[0x200], 1, file_size, file);
    fclose(file);
    return true;
}

void chip8_cycle(chip8_t *chip8) {
    uint16_t opcode = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc+1];
    chip8->pc += 2;


    uint8_t x = (opcode & 0x0F00) >> 8;  // 2nd nibble - register X
    uint8_t y = (opcode & 0x00F0) >> 4;  // 3rd nibble - register Y
    uint8_t n = opcode & 0x000F;          // 4th nibble - small constant
    uint8_t nn = opcode & 0x00FF;         // lower byte - byte constant
    uint16_t nnn = opcode & 0x0FFF;       // lower 12 bits - address

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                case 0x00E0: memset(chip8->display, 0, sizeof(chip8->display)); break;
                case 0x00EE: /* RET - return from subroutine */ break;
            }
            break;
        case 0x1000: chip8->pc = nnn; break;
        case 0x2000: /* CALL nnn - call subroutine */ break;
        case 0x3000: /* SE Vx, nn - skip if Vx == nn */ break;
        case 0x4000: /* SNE Vx, nn - skip if Vx != nn */ break;
        case 0x5000: /* SE Vx, Vy - skip if Vx == Vy */ break;
        case 0x6000: chip8->V[x] = nn; break;
        case 0x7000: chip8->V[x] += nn; break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0: /* LD Vx, Vy - Vx = Vy */ break;
                case 0x1: /* OR Vx, Vy - Vx |= Vy */ break;
                case 0x2: /* AND Vx, Vy - Vx &= Vy */ break;
                case 0x3: /* XOR Vx, Vy - Vx ^= Vy */ break;
                case 0x4: /* ADD Vx, Vy - Vx += Vy, VF = carry */ break;
                case 0x5: /* SUB Vx, Vy - Vx -= Vy, VF = !borrow */ break;
                case 0x6: /* SHR Vx - Vx >>= 1, VF = old bit 0 */ break;
                case 0x7: /* SUBN Vx, Vy - Vx = Vy - Vx, VF = !borrow */ break;
                case 0xE: /* SHL Vx - Vx <<= 1, VF = old bit 7 */ break;
            }
            break;
        case 0x9000: /* SNE Vx, Vy - skip if Vx != Vy */ break;
        case 0xA000: chip8->I = nnn; break;
        case 0xB000: /* JP V0, nnn - jump to nnn + V0 */ break;
        case 0xC000: /* RND Vx, nn - Vx = random & nn */ break;
        case 0xD000: {
            int X = chip8->V[x] % 64;
            int Y = chip8->V[y] % 32;
            chip8->V[0xF] = 0;

            for (int row = 0; row < n; row++) {
                uint8_t sprite_byte = chip8->memory[chip8->I + row];

                // CHIP-8 clips sprites at the screen edges, it does not wrap them
                if (Y + row >= 32) break;

                for (int col = 0; col < 8; col++) {
                    if (X + col >= 64) break; // Clip at right edge

                    uint8_t pixel = sprite_byte & (0x80 >> col);

                    // We just need to check if it is greater than 0.
                    if (pixel != 0) {

                        // Calculate the exact 1D index for your display array
                        int index = (Y + row) * 64 + (X + col);

                        // If the screen pixel is already ON, we are erasing it. Set collision!
                        if (chip8->display[index] == 1) {
                            chip8->V[0xF] = 1;
                        }

                        // XOR the sprite pixel onto the screen
                        chip8->display[index] ^= 1;
                    }
                }
            }

            // Set your flag so main.c knows it's time to update the SDL window
            chip8->draw_flag = true;
            break;
        }
        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x9E: /* SKP Vx - skip if key Vx pressed */ break;
                case 0xA1: /* SKNP Vx - skip if key Vx not pressed */ break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x07: /* LD Vx, DT - Vx = delay_timer */ break;
                case 0x0A: /* LD Vx, K - wait for key press */ break;
                case 0x15: /* LD DT, Vx - delay_timer = Vx */ break;
                case 0x18: /* LD ST, Vx - sound_timer = Vx */ break;
                case 0x1E: /* ADD I, Vx - I += Vx */ break;
                case 0x29: /* LD F, Vx - I = font address for Vx */ break;
                case 0x33: /* BCD Vx - store BCD of Vx at I */ break;
                case 0x55: /* LD [I], Vx - store V0..Vx at I */ break;
                case 0x65: /* LD Vx, [I] - load V0..Vx from I */ break;
            }
            break;
        default:
            printf("Unknown opcode: 0x%04X\n", opcode);
            break;
    }
}