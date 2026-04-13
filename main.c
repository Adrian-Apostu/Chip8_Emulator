#include <stdio.h>
#include "chip8.h"
#include "SDL2/SDL.h"

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: %s <path_to_rom>\n", argv[0]);
        return 1;
    }

    const char *rom_filename = argv[1];

    chip8_t chip8;

    chip8_init(&chip8);

    if (!chip8_load_rom(&chip8, rom_filename)) {
        printf("Failed to load ROM: %s\n", rom_filename);
        return 1;
    }

    printf("Success! ROM '%s' loaded into memory.\n", rom_filename);

    //create GUI
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 640, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (texture == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    int running = 1;
    SDL_Event event;

    //event loop
    uint32_t last_time = SDL_GetTicks();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                uint8_t state = (event.type == SDL_KEYDOWN) ? 1 : 0;
                switch (event.key.keysym.sym) {
                    case SDLK_1: chip8.keypad[0x1] = state; break;
                    case SDLK_2: chip8.keypad[0x2] = state; break;
                    case SDLK_3: chip8.keypad[0x3] = state; break;
                    case SDLK_4: chip8.keypad[0xC] = state; break;
                    case SDLK_q: chip8.keypad[0x4] = state; break;
                    case SDLK_w: chip8.keypad[0x5] = state; break;
                    case SDLK_e: chip8.keypad[0x6] = state; break;
                    case SDLK_r: chip8.keypad[0xD] = state; break;
                    case SDLK_a: chip8.keypad[0x7] = state; break;
                    case SDLK_s: chip8.keypad[0x8] = state; break;
                    case SDLK_d: chip8.keypad[0x9] = state; break;
                    case SDLK_f: chip8.keypad[0xE] = state; break;
                    case SDLK_z: chip8.keypad[0xA] = state; break;
                    case SDLK_x: chip8.keypad[0x0] = state; break;
                    case SDLK_c: chip8.keypad[0xB] = state; break;
                    case SDLK_v: chip8.keypad[0xF] = state; break;
                    case SDLK_ESCAPE: running = 0; break;
                }
            }
        }

        chip8_cycle(&chip8);

        uint32_t current_time = SDL_GetTicks();
        if ((current_time - last_time) >= 16) {
            if (chip8.delay_timer > 0) chip8.delay_timer--;
            if (chip8.sound_timer > 0) chip8.sound_timer--;
            last_time = current_time;
        }

        //If screen changed, update display
        SDL_RenderClear(renderer);
        if (chip8.draw_flag) {
            // convert display array to texture and render
            uint32_t pixels[64 * 32];
            for (int i = 0; i < 64 * 32; i++) {
                pixels[i] = chip8.display[i] ? 0xFFFFFFFF : 0x000000FF;
            }
            SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(uint32_t));
            chip8.draw_flag = false;
        }
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}