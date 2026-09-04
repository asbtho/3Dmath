#include <SDL3/SDL.h>
#include <chrono>
#include <iostream>
#include "math.h"
#include "engine.h"
#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    #ifdef _WIN32
        SetConsoleOutputCP(65001); // Enables UTF-8 encoding for Windows terminal
    #endif
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window;
    SDL_Renderer* renderer;
    window = SDL_CreateWindow("MatrixRenderer", 960, 540, SDL_WINDOW_RESIZABLE );
    renderer = SDL_CreateRenderer(window, NULL);

    bool running = true;
    SDL_Event Event;

    // Cube points
    std::vector<Vector3> cubePoints{
        { -1, -1, -1 }, {  1, -1, -1 }, {  1,  1, -1 }, { -1,  1, -1 },
        { -1, -1,  1 }, {  1, -1,  1 }, {  1,  1,  1 }, { -1,  1,  1 }
    };
    
    std::vector<Triangle> cubeTris{
        { 0, 2, 1 }, { 0, 3, 2 },   // -Z
        { 4, 5, 6 }, { 4, 6, 7 },   // +Z
        { 0, 4, 7 }, { 0, 7, 3 },   // -X
        { 1, 2, 6 }, { 1, 6, 5 },   // +X
        { 0, 1, 5 }, { 0, 5, 4 },   // -Y
        { 3, 7, 6 }, { 3, 6, 2 }    // +Y
    };

    Engine render(window, renderer, cubePoints, cubeTris);

    // Time tracking
    Uint64 LAST = SDL_GetPerformanceCounter();
    Uint64 NOW = SDL_GetPerformanceCounter();
    float deltaTime = 0.0f;
    // FPS Counter
    float fpsTimer = 0.0f;
    int frameCount = 0;
    int currentFPS = 0;
	char title[64];

    while (render.isRunning()) {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = static_cast<float>(NOW - LAST) / SDL_GetPerformanceFrequency();
        if (deltaTime > 0.1f) { deltaTime = 0.1f; } // cap lag spikes

        fpsTimer += deltaTime;
		frameCount++;

        if (fpsTimer >= 1.0f) {
			currentFPS = frameCount;
            frameCount = 0;
            fpsTimer -= 1.0f;
			SDL_snprintf(title, sizeof(title), "MatrixRenderer - %d FPS", currentFPS);
			SDL_SetWindowTitle(window, title);
		}

        render.handleEvents();
        render.update(deltaTime);
        render.render();
        //std::cout << "DeltaTime: " << deltaTime << " seconds\n";
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
