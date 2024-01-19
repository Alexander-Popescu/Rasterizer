#include "SDL.h"
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"
#include <vector>
#include "PixelBuffer.h"


#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

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
    int window_width = DEFAULT_WIDTH;
    int window_height = DEFAULT_HEIGHT;

    //pixel buffer, really just a texture with some extra stuff
    PixelBuffer* pixelBuffer = new PixelBuffer(renderer, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    //circular buffer for frametimes
    std::vector<float> frametimes;
    const int MAX_FRAMETIMES = 100;

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
                    SDL_GetWindowSize(window, &window_width, &window_height);
                    pixelBuffer->resize(window_width, window_height);
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
                window_width = (event.window.data2 / 9) * 16;
                window_height = event.window.data2;
                SDL_SetWindowSize(window, window_width, window_height);

                if (!lock_resolution)
                {
                    pixelBuffer->resize(window_width, window_height);
                }
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_MINUS)
            {
                //half resolution
                std::vector<int> size = pixelBuffer->getSize();
                pixelBuffer->resize(size[0] / 2, size[1] / 2);
                pixelBuffer->update(true);
                lock_resolution = true;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_EQUALS)
            {
                //double resolution
                std::vector<int> size = pixelBuffer->getSize();
                pixelBuffer->resize(size[0] * 2, size[1] * 2);
                pixelBuffer->update(true);
                lock_resolution = true;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE)
            {
                //reset to default
                pixelBuffer->resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
                lock_resolution = true;
                window_height = DEFAULT_HEIGHT;
                window_width = DEFAULT_WIDTH;
                SDL_SetWindowSize(window, DEFAULT_WIDTH, DEFAULT_HEIGHT);
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        int start_time = SDL_GetTicks();
        pixelBuffer->update(pause);
        int frametime = (SDL_GetTicks() - start_time);
        frametimes.push_back(frametime);

        //circulate frametime buffer
        if (frametimes.size() > MAX_FRAMETIMES) {
            frametimes.erase(frametimes.begin());
        }

        if(show_debug_window) {
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            ImGui::Begin("Debug Window");

            ImGui::Text("Current Rendering State (Toggle P): %s", pause ? "Paused" : "Running"); 
            ImGui::Text("resolution (Toggle L): %s", lock_resolution ? "Locked" : "Unlocked");
            ImGui::Text("hit L twice to render one frame");
            ImGui::Text("Window Resolution: %d x %d", window_width, window_height);
            std::vector<int> size = pixelBuffer->getSize();
            ImGui::Text("Texture Resolution: %d x %d", size[0], size[1]);
            ImGui::Text("Texture Aspect Ratio: %f", (float)size[0] / (float)size[1]);
            ImGui::Text("Window Aspect Ratio: %f", (float)window_width / (float)window_height);
            ImGui::Text("Predicted FPS based on FrameTime: %f", 1000.0f / frametime);
            ImGui::Text("Actual FPS: %f", ImGui::GetIO().Framerate);
            ImGui::Text("Frame Time: %ims", frametime);
            ImGui::PlotLines("FrameTimes (ms)", &frametimes[0], frametimes.size(), 0, NULL, 0.0f, 100.0f, ImVec2(0, 80));
            ImGui::Text("Graph is from 0 - 100 ms");
            ImGui::Text("Press Space to hide this window");
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
        SDL_RenderCopy(renderer, pixelBuffer->getTexture(), NULL, NULL);
        SDL_GL_SwapWindow(window);
    }
    
    // Cleanup
    delete pixelBuffer;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}