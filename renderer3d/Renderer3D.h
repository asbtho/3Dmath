#pragma once
#include <SDL3/SDL.h>
#include <cmath>
#include <chrono>
#include <vector>

struct Point2D {
    float x, y;
};

struct Point3D {
    float x, y, z;
};

struct Edge {
    int startIndex, endIndex;
};

// Renderer3D class handles the rendering of 3D objects onto a 2D SDL window.
class Renderer3D {
    public:
        Renderer3D(SDL_Window* window, SDL_Renderer* renderer, const std::vector<Point3D>& points, const std::vector<Edge>& edges);
        void render();
    private:
        Point3D rotateX(Point3D point);
        Point3D rotateY(Point3D point);
        Point2D projection(Point3D point);

        float rotationAngle = 0.0f;
        float focalLength = 10.0f;
        float DeltaTime = 0.0f;
        bool rotationXenabled = true;
        bool rotationYenabled = true;
        int scaleFactor = 100;

        int WindowSizeX;
        int WindowSizeY;
        SDL_Renderer* renderer;

        std::vector<Point3D> vertices;
        std::vector<Edge> edgeIndices;
};
