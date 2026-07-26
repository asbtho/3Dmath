#include "Renderer3D.h"

// Constructor for the Renderer3D class. Initializes the renderer with the provided SDL window, SDL renderer, points, and edges.
Renderer3D::Renderer3D(SDL_Window* _window, SDL_Renderer* _renderer, const std::vector<Point3D>& _points, const std::vector<Vertex>& _edges){
    SDL_GetWindowSize(_window, &WindowSizeX, &WindowSizeY);
    renderer = _renderer;
    points = _points;
    edges = _edges;
}

void Renderer3D::render(){
    auto time1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration(0);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    rotation += 1 * DeltaTime;
    //rotation = 1;
    rotationXenabled = false;
    rotationYenabled = true;

    for (const auto& edge : edges) {
        Point3D rotatedStartPoint = points[edge.start];
        Point3D rotatedEndPoint = points[edge.end];
        if (rotationXenabled && rotationYenabled) {
            rotatedStartPoint = rotateX(rotateY(points[edge.start]));
            rotatedEndPoint = rotateX(rotateY(points[edge.end]));
        } else if (rotationXenabled) {
            rotatedStartPoint = rotateX(points[edge.start]);
            rotatedEndPoint = rotateX(points[edge.end]);
        } else if (rotationYenabled) {
            rotatedStartPoint = rotateY(points[edge.start]);
            rotatedEndPoint = rotateY(points[edge.end]);
        } 
        Point2D start = projection(rotatedStartPoint);
        Point2D end = projection(rotatedEndPoint);
        SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
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
    rotatedPoint.y = point.y * cos(rotation) - point.z * sin(rotation);
    rotatedPoint.z = point.y * sin(rotation) + point.z * cos(rotation);
    return rotatedPoint;
}

Point3D Renderer3D::rotateY(Point3D point){
    Point3D rotatedPoint;
    rotatedPoint.x = point.x * cos(rotation) + point.z * sin(rotation);
    rotatedPoint.y = point.y;
    rotatedPoint.z = -point.x * sin(rotation) + point.z * cos(rotation);
    return rotatedPoint;
}

Point2D Renderer3D::projection(Point3D point){
    Point2D projectedPoint;
    projectedPoint.x = (point.x * focalLength) / (point.z + focalLength)*scaleFactor + WindowSizeX / 2;
    projectedPoint.y = (point.y * focalLength) / (point.z + focalLength)*scaleFactor + WindowSizeY / 2;
    return projectedPoint;
}
