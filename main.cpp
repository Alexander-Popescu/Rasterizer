#include "SDL.h"
#include <random>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"
//using the meson build system. Run "meson compile -C ../builddir" in the builddir

#define START_SCREEN_WIDTH 1500
#define START_SCREEN_HEIGHT 900


int RenderScene(SDL_Surface* pixelSurface) {
    Uint32* pixels = (Uint32*)pixelSurface->pixels;
    for (int y = 0; y < pixelSurface->h; ++y) {
        for (int x = 0; x < pixelSurface->w; ++x) {
            Uint32 pixel = (Uint32)rand();
            pixels[y * pixelSurface->w + x] = pixel;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{


    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Renderer",
        0,
        0,
        START_SCREEN_WIDTH,
        START_SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE //opengl for imgui debug windows, may remove later and switch to other ui library or make own
    );

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    //GPU acceleration flag
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        printf("Could not create renderer: %s\n", SDL_GetError());
        return 1;
    }

    SDL_DisplayMode dm;

    if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
        printf("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
        return 1;
    }

    SDL_GetCurrentDisplayMode(0, &dm);
    float refresh_rate = dm.refresh_rate; // in Hz
    float frame_delay = 1000 / refresh_rate; // in milliseconds

    //------End of SDL init stuff-------

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == NULL) {
        printf("Could not create OpenGL context: %s\n", SDL_GetError());
        return 1;
    }

    const char* glsl_version = "#version 130";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //individual pixel editing setup

    float resolution_scale = 1.0;

    // surface to contain pixels
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    SDL_Surface* pixelSurface = SDL_CreateRGBSurface(0, windowWidth * resolution_scale, windowHeight * resolution_scale, 32, 0, 0, 0, 0);

    if (pixelSurface == NULL) {
        printf("Could not create surface: %s\n", SDL_GetError());
        return 1;
    }

    
    SDL_Event event;
    bool running = true;
    bool isFullscreen = false;
    bool isPaused = false;
    bool single_frame = false;
    Uint32 frame_start = 0.0;
    Uint32 frame_time = 0.0;
    int frameCount = 0;
    float averageFPS = 0.0f;
    bool show_demo_window = true;
    Uint32 startTicks = SDL_GetTicks();
    while (running || isPaused) {
        if (single_frame) {
            isPaused = true;
            single_frame = false;
        }

        frame_start = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            //imgui inputs
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                running = false;
                isPaused = false;
            }
            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    windowWidth = event.window.data1;
                    windowHeight = event.window.data2;
                    SDL_FreeSurface(pixelSurface);
                    pixelSurface = SDL_CreateRGBSurface(0, windowWidth * resolution_scale, windowHeight * resolution_scale, 32, 0, 0, 0, 0);
                    if (isPaused)
                    {
                        single_frame = true;
                        isPaused = false;
                    }
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_F11) {
                    isFullscreen = !isFullscreen;
                    SDL_SetWindowFullscreen(window, isFullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                    if (isPaused)
                    {
                        single_frame = true;
                        isPaused = false;
                    }
                }
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                    isPaused = false;
                }
                if (event.key.keysym.sym == SDLK_p) {
                    isPaused = !isPaused;
                }
                if (event.key.keysym.sym == SDLK_MINUS) {
                    resolution_scale *= 1.1;
                    if (resolution_scale > 2.0) {
                        resolution_scale = 2.0;
                    }
                    SDL_FreeSurface(pixelSurface);
                    pixelSurface = SDL_CreateRGBSurface(0, windowWidth * resolution_scale, windowHeight * resolution_scale, 32, 0, 0, 0, 0);
                    if (isPaused)
                    {
                        single_frame = true;
                        isPaused = false;
                    }
                }
                if (event.key.keysym.sym == SDLK_EQUALS && event.key.keysym.mod & KMOD_SHIFT) {
                    resolution_scale *= 0.9;
                    if (resolution_scale < 0.01) {
                        resolution_scale = 0.01;
                    }
                    SDL_FreeSurface(pixelSurface);
                    pixelSurface = SDL_CreateRGBSurface(0, windowWidth * resolution_scale, windowHeight * resolution_scale, 32, 0, 0, 0, 0);
                    if (isPaused)
                    {
                        single_frame = true;
                        isPaused = false;
                    }
                }
                if (event.key.keysym.sym == SDLK_BACKQUOTE)
                {
                    show_demo_window = !show_demo_window;
                }
            }
        }

        SDL_RenderClear(renderer);
        
        if (!isPaused)
        {
            RenderScene(pixelSurface);
        }
        
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, pixelSurface);

        if (texture == NULL) {
            printf("Could not create texture: %s\n", SDL_GetError());
            return 1;
        }

        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_DestroyTexture(texture);

        //limit to refresh rate
        frameCount++;
        frame_time = SDL_GetTicks() - frame_start;

        if (frame_delay > frame_time)
        {
            SDL_Delay(frame_delay - frame_time);
        }

        if (SDL_GetTicks() - startTicks >= 1000) // If one second has passed
        {
            averageFPS = frameCount; // The number of frames processed in the last second is the FPS
            frameCount = 0;
            startTicks = SDL_GetTicks();
        }

        if (show_demo_window)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            ImGui::Begin("Debug Info");

            ImGui::Text("This is some useful text.");
            ImGui::Text("Paused State: %s", isPaused ? "true" : "false");

            ImGui::Text("resolution_scale = %f", resolution_scale);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / averageFPS, averageFPS);
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        SDL_RenderPresent(renderer);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext(ImGui::GetCurrentContext());

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}