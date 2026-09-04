#include <SDL3/SDL.h>
#include <cmath>
#include <vector>
#include <cstdint>
#include <cstring>
#include "math.h"

class Engine {
public:
    Engine(SDL_Window* window, SDL_Renderer* renderer, const std::vector<Vector3>& points, const std::vector<Triangle>& triangles);
    ~Engine();

	void render();
    void handleEvents();
    void update(float deltaTime);
    bool isRunning() const;
    void putPixel(int x, int y, uint32_t color);
    void drawLine(int x0, int y0, int x1, int y1, uint32_t color);

private:
    float rotationAngleY = 0.0f;
    float rotationAngleX = 0.0f;
    float objectScale = 1.0f;
    float focalLength = 10.0f;
    float moveSpeed = 3.0f;
    bool rotationXenabled = true;
    bool rotationYenabled = true;
    int scaleFactor = 200;
    Vector3 cameraPosition{ 0.0f, 0.0f, 0.0f };

    int WindowSizeX;
    int WindowSizeY;
    bool running = true;
    SDL_Event event;
    SDL_Renderer* renderer;
    SDL_Texture*          frameTex = nullptr;

    std::vector<uint32_t> framebuffer;   // ARGB8888
    std::vector<Vector3> points;
    std::vector<Triangle> triangles;

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
