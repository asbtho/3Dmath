#include <SDL3/SDL.h>
#include <cmath>
#include <vector>
#include "math.h"

class Engine {
public:
    Engine(SDL_Window* window, SDL_Renderer* renderer, const std::vector<Vector3>& points, const std::vector<Edge>& edges);

	void render();
    void handleEvents();
    void update(float deltaTime);
    bool isRunning() const;

private:
    float rotationAngleY = 0.0f;
    float rotationAngleX = 0.0f;
    float objectScale = 1.0f;
    float focalLength = 10.0f;
    bool rotationXenabled = true;
    bool rotationYenabled = true;
    int scaleFactor = 200;
    Vector3 cameraPosition{ 0.0f, 0.0f, 0.0f };
    float moveSpeed = 3.0f;

    int WindowSizeX;
    int WindowSizeY;
    SDL_Event event;
    SDL_Renderer* renderer;
    bool running = true;

    std::vector<Vector3> points;
    std::vector<Edge> edges;

    enum keys {
		LEFT = 0,
		RIGHT = 1,
		UP = 2,
        DOWN = 3,
        W = 4,
        A = 5,
        S = 6,
        D = 7,
        Q = 8,
        E = 9
	};
	bool key_state[10] = {};
};
