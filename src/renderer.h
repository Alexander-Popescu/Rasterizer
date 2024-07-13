#pragma once
#include <SDL.h>
#include <stdio.h>

class Renderer {
    public:

    Renderer(int width, int height, SDL_Surface* surface); //constructor
    ~Renderer(); //destructor
   
    SDL_Surface* surface;

    int renderFrame(); //renders frame and puts pixel data into surface
    void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b); //draws individual pixel to the SDL_Surface

    void signalChange(); // run if the scene has changed, renderer will render only static images, or will rerender a frame if
    // something has moved or another part of the program tells it to with this function

    private:

    bool changeSinceLastFrame = true;
    int width;
    int height;
    uint32_t* pixels_buffer; //buffer to hold pixel info before we blit to surface
};
