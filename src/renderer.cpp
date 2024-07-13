#include <cstdint>
#include <src/renderer.h>
#include <SDL.h>

Renderer::Renderer(int width, int height, SDL_Surface* surface) {
    this->width = width;
    this->height = height;

    this->surface = surface;

    this->pixels_buffer = new uint32_t[width * height];
}

int Renderer::renderFrame() {
    if (changeSinceLastFrame == false) { return 0; }

    SDL_LockSurface(surface);
    
   for (int y = 0; y < surface->h; ++y) {
        for (int x = 0; x < surface->w; ++x) {
            drawPixel(x, y, rand() << 6, rand() << 4, rand() << 2);
        }
    }

    SDL_UnlockSurface(surface);
    
    changeSinceLastFrame = false;
    return 0;
}

void Renderer::drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
        ((uint32_t*)surface->pixels)[(y * surface->w) + x] = SDL_MapRGBA(surface->format, r, g, b, 255);
}

void Renderer::signalChange() {
    changeSinceLastFrame = true;
}
