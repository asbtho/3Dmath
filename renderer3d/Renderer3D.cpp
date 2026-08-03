#include "Renderer3D.h"

// Constructor for the Renderer3D class. Initializes the renderer with the provided SDL window, SDL renderer, vertices, and edge indices.
Renderer3D::Renderer3D(SDL_Window* _window, SDL_Renderer* _renderer, const std::vector<Point3D>& _vertices, const std::vector<Edge>& _edgeIndices){
    SDL_GetWindowSize(_window, &WindowSizeX, &WindowSizeY);
    renderer = _renderer;
    vertices = _vertices;
    edgeIndices = _edgeIndices;
}

void Renderer3D::render(){
    auto time1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration(0);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    rotationAngle += 1 * DeltaTime;
    //rotation = 1;
    rotationXenabled = false;
    rotationYenabled = true;

    for (const auto& edge : edgeIndices) {
        Point3D rotatedStartPoint = vertices[edge.startIndex];
        Point3D rotatedEndPoint = vertices[edge.endIndex];
        if (rotationXenabled && rotationYenabled) {
            rotatedStartPoint = rotateX(rotateY(vertices[edge.startIndex]));
            rotatedEndPoint = rotateX(rotateY(vertices[edge.endIndex]));
        } else if (rotationXenabled) {
            rotatedStartPoint = rotateX(vertices[edge.startIndex]);
            rotatedEndPoint = rotateX(vertices[edge.endIndex]);
        } else if (rotationYenabled) {
            rotatedStartPoint = rotateY(vertices[edge.startIndex]);
            rotatedEndPoint = rotateY(vertices[edge.endIndex]);
        } 
        Point2D start = projection(rotatedStartPoint);
        Point2D end = projection(rotatedEndPoint);
        SDL_RenderLine(renderer, start.x, start.y, end.x, end.y);
    }
    SDL_RenderPresent(renderer);

    auto time2 = std::chrono::high_resolution_clock::now();
    duration = time2 - time1;
    DeltaTime = duration.count();
    time1 = time2;
}

Point3D Renderer3D::rotateX(Point3D point){
    Point3D rotatedPoint;
    rotatedPoint.x = point.x;
    rotatedPoint.y = point.y * cos(rotationAngle) - point.z * sin(rotationAngle);
    rotatedPoint.z = point.y * sin(rotationAngle) + point.z * cos(rotationAngle);
    return rotatedPoint;
}

Point3D Renderer3D::rotateY(Point3D point){
    Point3D rotatedPoint;
    rotatedPoint.x = point.x * cos(rotationAngle) + point.z * sin(rotationAngle);
    rotatedPoint.y = point.y;
    rotatedPoint.z = -point.x * sin(rotationAngle) + point.z * cos(rotationAngle);
    return rotatedPoint;
}

Point2D Renderer3D::projection(Point3D point){
    Point2D projectedPoint;
    projectedPoint.x = (point.x * focalLength) / (point.z + focalLength)*scaleFactor + WindowSizeX / 2;
    projectedPoint.y = (point.y * focalLength) / (point.z + focalLength)*scaleFactor + WindowSizeY / 2;
    return projectedPoint;
}
