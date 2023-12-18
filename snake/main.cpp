#include <iostream>
#include <cassert>
#include "cpu.h"
#include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
#include <random>
#include <chrono>
#include <thread>

SDL_Color color_constructor(uint8_t byte)
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

bool read_screen_state(CPU &cpu, uint8_t screen_state[32 * 3 * 32])
{
    bool update = false;
    int frame_idx = 0;
    for (int i = 0x0200; i < 0x600; ++i)
    {
        uint8_t color_idx = cpu.mem_read(i);
        SDL_Color color = color_constructor(color_idx); // Assuming you have a way to extract RGB from SDL_Color
        uint8_t r = color.r;
        uint8_t g = color.g;
        uint8_t b = color.b;
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

void handle_user_input(CPU &cpu, SDL_Event &event)
{
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                exit(0);
            }

            // if W is pressed
            else if (event.key.keysym.sym == SDLK_w)
            {
                cpu.mem_write(0xfe, 0x77);
            }
            // if A is pressed
            else if (event.key.keysym.sym == SDLK_a)
            {
                cpu.mem_write(0xfe, 0x61);
            }
            // if S is pressed
            else if (event.key.keysym.sym == SDLK_s)
            {
                cpu.mem_write(0xfe, 0x73);
            }
            // if D is pressed
            else if (event.key.keysym.sym == SDLK_d)
            {
                cpu.mem_write(0xfe, 0x64);
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

    // ... Initialization of CPU and game code ...

    std::vector<uint8_t> game_code = {
        0x20, 0x06, 0x06, 0x20, 0x38, 0x06, 0x20, 0x0d, 0x06, 0x20, 0x2a, 0x06, 0x60, 0xa9, 0x02, 0x85,
        0x02, 0xa9, 0x04, 0x85, 0x03, 0xa9, 0x11, 0x85, 0x10, 0xa9, 0x10, 0x85, 0x12, 0xa9, 0x0f, 0x85,
        0x14, 0xa9, 0x04, 0x85, 0x11, 0x85, 0x13, 0x85, 0x15, 0x60, 0xa5, 0xfe, 0x85, 0x00, 0xa5, 0xfe,
        0x29, 0x03, 0x18, 0x69, 0x02, 0x85, 0x01, 0x60, 0x20, 0x4d, 0x06, 0x20, 0x8d, 0x06, 0x20, 0xc3,
        0x06, 0x20, 0x19, 0x07, 0x20, 0x20, 0x07, 0x20, 0x2d, 0x07, 0x4c, 0x38, 0x06, 0xa5, 0xff, 0xc9,
        0x77, 0xf0, 0x0d, 0xc9, 0x64, 0xf0, 0x14, 0xc9, 0x73, 0xf0, 0x1b, 0xc9, 0x61, 0xf0, 0x22, 0x60,
        0xa9, 0x04, 0x24, 0x02, 0xd0, 0x26, 0xa9, 0x01, 0x85, 0x02, 0x60, 0xa9, 0x08, 0x24, 0x02, 0xd0,
        0x1b, 0xa9, 0x02, 0x85, 0x02, 0x60, 0xa9, 0x01, 0x24, 0x02, 0xd0, 0x10, 0xa9, 0x04, 0x85, 0x02,
        0x60, 0xa9, 0x02, 0x24, 0x02, 0xd0, 0x05, 0xa9, 0x08, 0x85, 0x02, 0x60, 0x60, 0x20, 0x94, 0x06,
        0x20, 0xa8, 0x06, 0x60, 0xa5, 0x00, 0xc5, 0x10, 0xd0, 0x0d, 0xa5, 0x01, 0xc5, 0x11, 0xd0, 0x07,
        0xe6, 0x03, 0xe6, 0x03, 0x20, 0x2a, 0x06, 0x60, 0xa2, 0x02, 0xb5, 0x10, 0xc5, 0x10, 0xd0, 0x06,
        0xb5, 0x11, 0xc5, 0x11, 0xf0, 0x09, 0xe8, 0xe8, 0xe4, 0x03, 0xf0, 0x06, 0x4c, 0xaa, 0x06, 0x4c,
        0x35, 0x07, 0x60, 0xa6, 0x03, 0xca, 0x8a, 0xb5, 0x10, 0x95, 0x12, 0xca, 0x10, 0xf9, 0xa5, 0x02,
        0x4a, 0xb0, 0x09, 0x4a, 0xb0, 0x19, 0x4a, 0xb0, 0x1f, 0x4a, 0xb0, 0x2f, 0xa5, 0x10, 0x38, 0xe9,
        0x20, 0x85, 0x10, 0x90, 0x01, 0x60, 0xc6, 0x11, 0xa9, 0x01, 0xc5, 0x11, 0xf0, 0x28, 0x60, 0xe6,
        0x10, 0xa9, 0x1f, 0x24, 0x10, 0xf0, 0x1f, 0x60, 0xa5, 0x10, 0x18, 0x69, 0x20, 0x85, 0x10, 0xb0,
        0x01, 0x60, 0xe6, 0x11, 0xa9, 0x06, 0xc5, 0x11, 0xf0, 0x0c, 0x60, 0xc6, 0x10, 0xa5, 0x10, 0x29,
        0x1f, 0xc9, 0x1f, 0xf0, 0x01, 0x60, 0x4c, 0x35, 0x07, 0xa0, 0x00, 0xa5, 0xfe, 0x91, 0x00, 0x60,
        0xa6, 0x03, 0xa9, 0x00, 0x81, 0x10, 0xa2, 0x00, 0xa9, 0x01, 0x81, 0x10, 0x60, 0xa2, 0x00, 0xea,
        0xea, 0xca, 0xd0, 0xfb, 0x60};

    CPU cpu;
    cpu.load(game_code);
    cpu.reset();

    uint8_t screen_state[32 * 3 * 32] = {0};
    std::default_random_engine rng;
    std::uniform_int_distribution<uint8_t> dist(1, 15);

    SDL_Event event;

    cpu.run_with_callback([&](CPU &cpu)
                          {
                              handle_user_input(cpu, event);

        cpu.mem_write(0xfe, dist(rng));

        if (read_screen_state(cpu, screen_state)) {
            SDL_UpdateTexture(texture, nullptr, screen_state, 32 * 3);
            SDL_RenderCopy(canvas, texture, nullptr, nullptr);
            SDL_RenderPresent(canvas);
        }

        std::this_thread::sleep_for(std::chrono::microseconds(70000)); });

    // Clean up
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(canvas);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
