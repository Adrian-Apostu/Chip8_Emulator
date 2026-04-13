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
        SDL_Delay(2);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}