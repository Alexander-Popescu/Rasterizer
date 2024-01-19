// PixelBuffer.h
#pragma once
#include "SDL.h"
#include <cstdint>
#include <vector>

class PixelBuffer {
public:
    PixelBuffer(SDL_Renderer* renderer, int width, int height);
    ~PixelBuffer();

    void update(bool pause);
    uint32_t* getBuffer();
    SDL_Texture* getTexture();
    std::vector<int> getSize();
    void resize(int newWidth, int newHeight);

private:
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    uint32_t* pixel_buffer_buffer;
    int width, height;
};