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
}

void Engine::render(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    std::cout << "Rotation X: " << rotationAngleX << "\xC2\xB0 " 
          << "Rotation Y: " << rotationAngleY << "\xC2\xB0\n";
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
        SDL_RenderLine(renderer, screen[t.a].x, screen[t.a].y, screen[t.b].x, screen[t.b].y);
        SDL_RenderLine(renderer, screen[t.b].x, screen[t.b].y, screen[t.c].x, screen[t.c].y);
        SDL_RenderLine(renderer, screen[t.c].x, screen[t.c].y, screen[t.a].x, screen[t.a].y);
    }

    // Draw original points as well
    for (size_t i = 0; i < points.size(); ++i) {
        if (visible[i]) SDL_RenderPoint(renderer, screen[i].x, screen[i].y);
    }

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
