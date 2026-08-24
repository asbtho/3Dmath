#include "engine.h"

constexpr double PI = 3.14159265358979323846; 

constexpr double degreesToRadians(double degrees) {
    return degrees * (PI / 180.0);
}

Engine::Engine(SDL_Window* window, SDL_Renderer* renderer, const std::vector<Vector3>& points, const std::vector<Edge>& edges){
    this->WindowSizeX = SDL_GetWindowSurface(window)->w;
    this->WindowSizeY = SDL_GetWindowSurface(window)->h;
    this->renderer = renderer;
    this->points = points;
    this->edges = edges;
}

void Engine::render(float deltaTime){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    rotationAngle += 5 * deltaTime; // Rotate at 1 degree per second
    std::cout << "Rotation Angle: " << rotationAngle << " degrees\n";

    // Do the transformations
    Matrix44 scale     = Matrix44::scaling(1.0f, 1.0f, 1.0f);       // half size
    Matrix44 rotation  = Matrix44::rotationY(degreesToRadians(rotationAngle));    // rotate around Y-axis
    Matrix44 translate = Matrix44::translation(0.0f, 0.0f, -5.0f);   // move 5 units into the screen (negative Z direction)
    // Combine the transformations into a single matrix. Order: scale -> rotate -> translate
    Matrix44 worldMatrix = translate * rotation * scale;
    // Create perspective (FOV: 60 degrees, Aspect Ratio: 16/9, Near: 0.1, Far: 100)
    float fov = degreesToRadians(60.0);
    Matrix44 projectionMatrix = Matrix44::perspective(fov, 16.0f / 9.0f, 0.1f, 100.0f);
    // Combine to a single matrix
    Matrix44 finalMVP = projectionMatrix * worldMatrix;

    // Project every 3D vertex once into 2D screen space.
    std::vector<Vector2> screen(points.size());
    std::vector<bool>    visible(points.size(), false);

    for (size_t i = 0; i < points.size(); ++i) {
        Vector4 clip = finalMVP.transformH(points[i]);
        if (clip.w <= 0.0f) continue; // behind the camera

        float ndcX = clip.x / clip.w;
        float ndcY = clip.y / clip.w;

        screen[i]  = { ndcX * scaleFactor + WindowSizeX / 2.0f,
                       ndcY * scaleFactor + WindowSizeY / 2.0f };
        visible[i] = true;
    }

    // Draw each edge between the two projected endpoints
    for (const Edge& e : edges) {
        if (!visible[e.a] || !visible[e.b]) continue;
        SDL_RenderLine(renderer, screen[e.a].x, screen[e.a].y, screen[e.b].x, screen[e.b].y);
    }

    // Draw original points as well
    for (size_t i = 0; i < points.size(); ++i) {
        if (visible[i]) SDL_RenderPoint(renderer, screen[i].x, screen[i].y);
    }

    SDL_RenderPresent(renderer);
}
