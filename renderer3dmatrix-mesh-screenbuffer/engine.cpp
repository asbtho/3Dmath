#include "engine.h"

constexpr double PI = 3.14159265358979323846; 

constexpr double degreesToRadians(double degrees) {
    return degrees * (PI / 180.0);
}

Engine::Engine(SDL_Window* window, SDL_Renderer* renderer, const std::vector<Vector3>& points, const std::vector<Triangle>& triangles){
    this->WindowSizeX = SDL_GetWindowSurface(window)->w;
    this->WindowSizeY = SDL_GetWindowSurface(window)->h;
    this->renderer = renderer;
    this->points = points;
    this->triangles = triangles;

    framebuffer.assign(size_t(WindowSizeX) * WindowSizeY, 0xFF202020u);

    frameTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WindowSizeX, WindowSizeY);
    SDL_SetTextureScaleMode(frameTex, SDL_SCALEMODE_NEAREST);
}

Engine::~Engine() {
    if (frameTex) SDL_DestroyTexture(frameTex);
}

void Engine::render(){
    std::fill(framebuffer.begin(), framebuffer.end(), 0xFF202020u);

    std::cout << "Rotation X: " << rotationAngleX << "\xC2\xB0 " << "Rotation Y: " << rotationAngleY << "\xC2\xB0\n";
    // Do the transformations
    Matrix44 scale     = Matrix44::scaling(objectScale, objectScale, objectScale); // scale the object
    Matrix44 rotationY = Matrix44::rotationY(degreesToRadians(rotationAngleY));    // rotate around Y-axis
    Matrix44 rotationX = Matrix44::rotationX(degreesToRadians(rotationAngleX));    // rotate around X-axis
    Matrix44 objectTranslation = Matrix44::translation(0.0f, 0.0f, -5.0f);
    Matrix44 cameraTranslation = Matrix44::translation(-cameraPosition.x,-cameraPosition.y,-cameraPosition.z);
    // Combine the transformations into a single matrix. Order: scale -> rotate -> translate
    Matrix44 worldMatrix = cameraTranslation * objectTranslation * rotationY * rotationX * scale;
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

    // Draw triangles based on the projected points
    for (const Triangle& t : triangles) {
        if (!visible[t.a] || !visible[t.b] || !visible[t.c]) continue;
        drawLine(int(screen[t.a].x), int(screen[t.a].y),
                 int(screen[t.b].x), int(screen[t.b].y), 0xFFFFFFFFu);
        drawLine(int(screen[t.b].x), int(screen[t.b].y),
                 int(screen[t.c].x), int(screen[t.c].y), 0xFFFFFFFFu);
        drawLine(int(screen[t.c].x), int(screen[t.c].y),
                 int(screen[t.a].x), int(screen[t.a].y), 0xFFFFFFFFu);
    }

    // 3x3 red dot at each visible corner so vertices stand out.
    for (size_t i = 0; i < points.size(); ++i) {
        if (!visible[i]) continue;
        int px = int(screen[i].x);
        int py = int(screen[i].y);
        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx)
                putPixel(px + dx, py + dy, 0xFFFF3030u);
    }

    void* pixels = nullptr;
    int pitch = 0;
    if (SDL_LockTexture(frameTex, nullptr, &pixels, &pitch)) {
        const size_t rowBytes = size_t(WindowSizeX) * sizeof(uint32_t);
        for (int y = 0; y < WindowSizeY; ++y) {
            std::memcpy(static_cast<uint8_t*>(pixels) + size_t(y) * pitch,
                        framebuffer.data() + size_t(y) * WindowSizeX,
                        rowBytes);
        }
        SDL_UnlockTexture(frameTex);
    }
    SDL_RenderTexture(renderer, frameTex, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Engine::update(float deltaTime) {
    if (key_state[LEFT])  rotationAngleY -= 10 * deltaTime; // Rotate at 10 degree per second
    if (key_state[RIGHT]) rotationAngleY += 10 * deltaTime; // Rotate at 10 degree per second
    if (key_state[UP])    rotationAngleX -= 10 * deltaTime; // Rotate at 10 degree per second
    if (key_state[DOWN])  rotationAngleX += 10 * deltaTime; // Rotate at 10 degree per second
    if (key_state[E])     objectScale += 1.0f * deltaTime;  // Scale up at 1 unit per second
    if (key_state[Q])     objectScale -= 1.0f * deltaTime;  // Scale down at 1 unit per second
    if (key_state[W]) cameraPosition.z -= moveSpeed * deltaTime;
    if (key_state[S]) cameraPosition.z += moveSpeed * deltaTime;
    if (key_state[A]) cameraPosition.x -= moveSpeed * deltaTime;
    if (key_state[D]) cameraPosition.x += moveSpeed * deltaTime;
}

void Engine::handleEvents() {
	while(SDL_PollEvent(&event)){
        if (event.type == SDL_EVENT_QUIT) { 
            running = false;
        }
		if (event.type == SDL_EVENT_KEY_DOWN) {
			if (event.key.scancode == SDL_SCANCODE_UP) {
				key_state[UP] = true;
			}
			if (event.key.scancode == SDL_SCANCODE_LEFT) {
				key_state[LEFT] = true;
			}
            if (event.key.scancode == SDL_SCANCODE_DOWN) {
                key_state[DOWN] = true;
            }
			if (event.key.scancode == SDL_SCANCODE_RIGHT) {
				key_state[RIGHT] = true;
			}
            if (event.key.scancode == SDL_SCANCODE_W){
                key_state[W] = true;
            }
            if (event.key.scancode == SDL_SCANCODE_A){
                key_state[A] = true;
            }
            if (event.key.scancode == SDL_SCANCODE_S){
                key_state[S] = true;
            }
            if (event.key.scancode == SDL_SCANCODE_D){
                key_state[D] = true;
            }
            if (event.key.scancode == SDL_SCANCODE_Q){
                key_state[Q] = true;
            }
            if (event.key.scancode == SDL_SCANCODE_E){
                key_state[E] = true;
            }
		}
		if (event.type == SDL_EVENT_KEY_UP) {
			if (event.key.scancode == SDL_SCANCODE_UP) {
				key_state[UP] = false;
			}
			if (event.key.scancode == SDL_SCANCODE_LEFT) {
				key_state[LEFT] = false;
			}
            if (event.key.scancode == SDL_SCANCODE_DOWN) {
                key_state[DOWN] = false;
            }
			if (event.key.scancode == SDL_SCANCODE_RIGHT) {
				key_state[RIGHT] = false;
			}
            if (event.key.scancode == SDL_SCANCODE_W) {
                key_state[W] = false;
            }
            if (event.key.scancode == SDL_SCANCODE_A) {
                key_state[A] = false;
            }
            if (event.key.scancode == SDL_SCANCODE_S) {
                key_state[S] = false;
            }
            if (event.key.scancode == SDL_SCANCODE_D) {
                key_state[D] = false;
            }
            if (event.key.scancode == SDL_SCANCODE_Q) {
                key_state[Q] = false;
            }
            if (event.key.scancode == SDL_SCANCODE_E) {
                key_state[E] = false;
            }
		}
	}
}

bool Engine::isRunning() const {
    return running;
}

void Engine::putPixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= WindowSizeX || y < 0 || y >= WindowSizeY) return;
    framebuffer[size_t(y) * WindowSizeX + x] = color;
}

// Integer Bresenham line, handles all octants.
void Engine::drawLine(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx =  std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    while (true) {
        putPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}
