// PixelBuffer.cpp
#include "PixelBuffer.h"

PixelBuffer::PixelBuffer(SDL_Renderer* renderer, int width, int height) {
    this->renderer = renderer;
    this->width = width;
    this->height = height;
    this->pixel_buffer_buffer = new uint32_t[width * height];
    this->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
}

PixelBuffer::~PixelBuffer() {
    delete[] pixel_buffer_buffer;
    SDL_DestroyTexture(texture);
}

std::vector<int> PixelBuffer::getSize() {
    return {width, height};
}

void PixelBuffer::resize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    delete[] pixel_buffer_buffer;
    pixel_buffer_buffer = new uint32_t[width * height];
    SDL_DestroyTexture(texture);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
}

void PixelBuffer::update(bool pause) {
    if (pause)
    {
        SDL_GL_BindTexture(texture, NULL, NULL);
        return; 
    }

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; ++j)
        {
            //pixel index
            int index = (j * width) + i;

            //pixel color
            uint32_t color = 0x00000000;

            //pixel position
            float x = (float)i / (float)width;
            float y = (float)j / (float)height;

            //pixel color
            color = (uint32_t)(x * 255) << 24 | (uint32_t)(y * 255) << 16 | 0x000000FF;

            //set pixel
            pixel_buffer_buffer[index] = color;
        }
    }

    SDL_UpdateTexture(texture, NULL, pixel_buffer_buffer, width * sizeof(uint32_t));
}

uint32_t* PixelBuffer::getBuffer() {
    return pixel_buffer_buffer;
}

SDL_Texture* PixelBuffer::getTexture() {
    return texture;
}
