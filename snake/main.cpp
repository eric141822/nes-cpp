#include <iostream>
#include <cassert>
#include "CPU.h"
#include <SDL2/SDL.h>
#include <random>
#include <chrono>
#include <thread>
#include <string>
#include <fstream>
#include <cassert>

const std::string FILE_NAME = "../snake/snake.nes";

std::vector<NES::u8> read_rom(const std::string &file)
{
    std::ifstream rom_file(file, std::ios::binary);

    if (!rom_file)
    {
        std::cerr << "Failed to open file: " << file << std::endl;
        exit(1);
    }

    std::vector<NES::u8> rom_data((std::istreambuf_iterator<char>(rom_file)),
                             std::istreambuf_iterator<char>());

    rom_file.close();

    return rom_data;
}

SDL_Color color_constructor(NES::u8 byte)
{
    switch (byte)
    {
    // BLACK
    case 0:
        return SDL_Color{0, 0, 0, 255};
    // WHITE
    case 1:
        return SDL_Color{255, 255, 255, 255};
    // GREY
    case 2:
    case 9:
        return SDL_Color{85, 85, 85, 255};
    // RED
    case 3:
    case 10:
        return SDL_Color{255, 0, 0, 255};
    // GREEN
    case 4:
    case 11:
        return SDL_Color{0, 255, 0, 255};

    // BLUE
    case 5:
    case 12:
        return SDL_Color{0, 0, 255, 255};

    // MAGENTA
    case 6:
    case 13:
        return SDL_Color{255, 0, 255, 255};
    // YELLOW
    case 7:
    case 14:
        return SDL_Color{255, 255, 0, 255};
    // CYAN
    default:
        return SDL_Color{0, 255, 255, 255};
    }
}

bool read_screen_state(NES::CPU &CPU, NES::u8 screen_state[32 * 3 * 32])
{
    bool update = false;
    int frame_idx = 0;
    for (int i = 0x0200; i < 0x0600; ++i)
    {
        NES::u8 color_idx = CPU.mem_read(i);
        SDL_Color color = color_constructor(color_idx);
        NES::u8 r = color.r;
        NES::u8 g = color.g;
        NES::u8 b = color.b;
        if (screen_state[frame_idx] != r || screen_state[frame_idx + 1] != g || screen_state[frame_idx + 2] != b)
        {
            screen_state[frame_idx] = r;
            screen_state[frame_idx + 1] = g;
            screen_state[frame_idx + 2] = b;
            update = true;
        }
        frame_idx += 3;
    }
    return update;
}

void handle_user_input(NES::CPU &CPU, SDL_Event &event)
{
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
        {
            exit(0);
            break;
        }
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                exit(0);
            }

            // if W is pressed
            else if (event.key.keysym.sym == SDLK_w)
            {
                CPU.mem_write(0xff, 0x77);
            }
            // if A is pressed
            else if (event.key.keysym.sym == SDLK_a)
            {
                CPU.mem_write(0xff, 0x61);
            }
            // if S is pressed
            else if (event.key.keysym.sym == SDLK_s)
            {
                CPU.mem_write(0xff, 0x73);
            }
            // if D is pressed
            else if (event.key.keysym.sym == SDLK_d)
            {
                CPU.mem_write(0xff, 0x64);
            }
        default:
            // do nothing
            break;
        }
    }
}

int main()
{

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create a window
    SDL_Window *window = SDL_CreateWindow("Snake Game",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          static_cast<int>(32.0 * 10.0),
                                          static_cast<int>(32.0 * 10.0),
                                          SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create a renderer (canvas)
    SDL_Renderer *canvas = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!canvas)
    {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Set scale for rendering
    SDL_RenderSetScale(canvas, 10.0, 10.0);

    // Create a texture
    SDL_Texture *texture = SDL_CreateTexture(canvas, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, 32, 32);
    if (!texture)
    {
        std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(canvas);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::vector<NES::u8> rom_code = read_rom(FILE_NAME);

    NES::Rom rom(rom_code);
    NES::Bus bus(rom);
    NES::CPU CPU(bus);
    CPU.reset();

    NES::u8 screen_state[32 * 3 * 32] = {0};
    std::default_random_engine rng;
    std::uniform_int_distribution<NES::u8> dist(1, 15);

    SDL_Event event;

    CPU.run_with_callback([&](NES::CPU &CPU)
                          {
                                                    handle_user_input(CPU, event);

                              CPU.mem_write(0xfe, dist(rng));

                              if (read_screen_state(CPU, screen_state)) {
                                  SDL_UpdateTexture(texture, nullptr, screen_state, 32 * 3);
                                  SDL_RenderCopy(canvas, texture, nullptr, nullptr);
                                  SDL_RenderPresent(canvas);
                              }

                              std::this_thread::sleep_for(std::chrono::microseconds(50)); });

    // Clean up
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(canvas);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
