#include <SDL3/SDL.h>
#include <cmath>
#include <vector>
#include "math.h"

class Engine {
public:
    Engine(SDL_Window* window, SDL_Renderer* renderer, const std::vector<Vector3>& points, const std::vector<Edge>& edges);

	void render(float deltaTime);

private:
    float rotationAngle = 0.0f;
    float focalLength = 10.0f;
    bool rotationXenabled = true;
    bool rotationYenabled = true;
    int scaleFactor = 200;

    int WindowSizeX;
    int WindowSizeY;
    SDL_Renderer* renderer;

    std::vector<Vector3> points;
    std::vector<Edge> edges;
};
