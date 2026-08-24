#include <SDL3/SDL.h>
#include <chrono>
#include <iostream>
#include "math.h"
#include "engine.h"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window;
    SDL_Renderer* renderer;
    window = SDL_CreateWindow("MatrixRenderer", 960, 540, SDL_WINDOW_RESIZABLE );
    renderer = SDL_CreateRenderer(window, NULL);

    bool running = true;
    SDL_Event Event;

    // Cube points
    std::vector<Vector3> cubePoints{ 
        Vector3{ -1.0f, -1.0f, -1.0f }, Vector3{ -1.0f, -1.0f, 1.0f },
        Vector3{ 1.0f, -1.0f, -1.0f },  Vector3{ -1.0f, 1.0f, -1.0f },
        Vector3{ -1.0f, 1.0f, 1.0f },   Vector3{ 1.0f, -1.0f, 1.0f },
        Vector3{ 1.0f, 1.0f, -1.0f },   Vector3{ 1.0f, 1.0f, 1.0f }
    };
    
    std::vector<Edge> cubeEdges{
        Edge{0,1}, Edge{0,2}, Edge{0,3},    // from corner 0
        Edge{1,4}, Edge{1,5},               // from corner 1
        Edge{2,5}, Edge{2,6},               // from corner 2
        Edge{3,4}, Edge{3,6},               // from corner 3
        Edge{4,7}, Edge{5,7}, Edge{6,7}     // to corner 7
    };

    Engine render(window, renderer, cubePoints, cubeEdges);

    // Time tracking
    Uint64 LAST = SDL_GetPerformanceCounter();
    Uint64 NOW = SDL_GetPerformanceCounter();
    float deltaTime = 0.0f;
    // FPS Counter
    float fpsTimer = 0.0f;
    int frameCount = 0;
    int currentFPS = 0;
	char title[64];

    while (running) {
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

        while (SDL_PollEvent(&Event)) {
            if (Event.type == SDL_EVENT_QUIT) { 
                running = false;
            }
        }

        if (!running) {
            break;
        }

        render.render(deltaTime);
        //std::cout << "DeltaTime: " << deltaTime << " seconds\n";
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
