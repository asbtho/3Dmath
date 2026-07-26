#include "Renderer3D.h"
#include <vector>
#include <cstdio>
#include <SDL3/SDL.h>

// Cube points and edges
std::vector<Point3D> points{ Point3D{ -1.0f, -1.0f, -1.0f }, Point3D{ -1.0f, -1.0f, 1.0f },
                            Point3D{ 1.0f, -1.0f, -1.0f }, Point3D{ -1.0f, 1.0f, -1.0f },
                            Point3D{ -1.0f, 1.0f, 1.0f }, Point3D{ 1.0f, -1.0f, 1.0f },
                            Point3D{ 1.0f, 1.0f, -1.0f }, Point3D{ 1.0f, 1.0f, 1.0f }};

std::vector<Vertex> edges{ Vertex{0, 1}, Vertex{0, 2}, Vertex{0, 3},
                            Vertex{2, 5}, Vertex{3, 6}, Vertex{3, 4},
                            Vertex{4, 7}, Vertex{6, 7}, Vertex{7, 5},
                            Vertex{5, 1}, Vertex{4, 1}, Vertex{2, 6} };


int main(int argc, char** argv){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window;
    SDL_Renderer* renderer;
    window = SDL_CreateWindow("3D Renderer", 960, 540, SDL_WINDOW_RESIZABLE );
    renderer = SDL_CreateRenderer(window, NULL);

    bool running = true;
    SDL_Event Event;

    Renderer3D renderer3D1(window, renderer, points, edges);

    while (running) {
        // Process pending OS/SDL events
        while (SDL_PollEvent(&Event)) {
            if (Event.type == SDL_EVENT_QUIT) { 
                running = false;
            }
        }

        if (!running) {
            break;
        }

        renderer3D1.render();
        printf("Rendering...\n");
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
