#include "SDL.h"
#include <random>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

void update_pixel_buffer(SDL_Texture* texture, bool pause)
{
    if (pause)
    {
        SDL_GL_BindTexture(texture, NULL, NULL);
        return;
    }
    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);

    uint32_t* pixels = new uint32_t[width * height];
    for (int i = 0; i < width * height; i++)
    {
        uint32_t rand = (uint32_t)std::rand();
        pixels[i] = rand;
    }
    SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(uint32_t));
    delete[] pixels;
}

int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Setup window
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    SDL_Window* window = SDL_CreateWindow("Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

    //OpenGL context
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    //Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Setup style
    ImGui::StyleColorsDark();
    
    // Main loop
    bool done = false;
    bool show_debug_window = true;
    bool lock_resolution = false;
    bool pause = false;

    //store window resolution for quick access
    int window_width = 1280;
    int window_height = 720;

    //pixel buffer
    SDL_Texture* pixel_buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 1280, 720);


    while(!done)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if(event.type == SDL_QUIT)
            {
                done = true;
            }
            if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            {
                done = true;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            {
                done = true;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
            {
                show_debug_window = !show_debug_window;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_l)
            {
                if (lock_resolution)
                {
                    SDL_DestroyTexture(pixel_buffer);
                    pixel_buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, window_height, window_width);
                    update_pixel_buffer(pixel_buffer, false);
                }
                lock_resolution = !lock_resolution;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p)
            {
                pause = !pause;
            }
            //resize window event
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                if (!lock_resolution)
                {
                    SDL_DestroyTexture(pixel_buffer);
                    pixel_buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, event.window.data1, event.window.data2);
                    window_width = event.window.data1;
                    window_height = event.window.data2;
                    update_pixel_buffer(pixel_buffer, false);
                }
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        update_pixel_buffer(pixel_buffer, pause);

        if(show_debug_window) {
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            ImGui::Begin("Hello, world!");
            ImGui::Text("Current Rendering State (Toggle P): %s", pause ? "Paused" : "Running"); 
            ImGui::Text("resolution (Toggle L): %s", lock_resolution ? "Locked" : "Unlocked");
            ImGui::Text("Window Resolution: %d x %d", window_width, window_height);
            ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
            ImGui::End();

            // Rendering
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        //render texture
        int display_w, display_h;
        SDL_GetWindowSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, pixel_buffer, NULL, NULL);
        SDL_GL_SwapWindow(window);
    }
    
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}