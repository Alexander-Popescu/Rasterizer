#include "SDL.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

int main() {

    //SDL init
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    //create an SDL window
    SDL_Window* window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DEFAULT_WIDTH, DEFAULT_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetSwapInterval(1);

    // create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return 1;
    }
    
    // This surface will act as our renderer's pixel buffer
    SDL_Surface* buffer = SDL_CreateRGBSurface(0,DEFAULT_WIDTH, DEFAULT_HEIGHT,32,0,0,0,0);

    //fill the surface with white pixels for now
    SDL_FillRect(buffer, NULL, 0xFFFFFFFF);

    //texture for actually rendering to the screen

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, buffer);
    if (texture == nullptr) {
        printf("Error in SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        return 1;
    }

    //main loop
    bool done = false;
    SDL_Event event;
    while (!done) {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                done = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    done = true;
                }
                break;
            }
        }

        //add noise to texture for testing
        SDL_LockSurface(buffer);
        uint32_t* pixels = (uint32_t*)buffer->pixels;
        for(int i = 0; i < (buffer->w * buffer->h); i++) {
            pixels[i] = SDL_MapRGBA(buffer->format, rand(), rand(), rand(), 255);
        }
        SDL_UnlockSurface(buffer);

        //update texture with surface data
        SDL_UpdateTexture(texture, NULL, buffer->pixels, sizeof(uint32_t) * buffer->w);

        //Render texture to the screen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
