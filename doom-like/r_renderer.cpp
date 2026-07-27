#include "r_renderer.h"

SDL_Window* window;
SDL_Renderer* sdl_renderer;
SDL_Texture* screen_texture;
unsigned int scrnw, scrnh;

bool is_debug_mode = false;
unsigned int *screen_buffer = NULL;
int screen_buffer_size = 0;

void R_ShutdownScreen(){
    if (screen_texture){
        SDL_DestroyTexture(screen_texture);
    }

    if (screen_buffer != NULL){
        free(screen_buffer);
    }
}

void R_Shutdown(){
    R_ShutdownScreen();
    SDL_DestroyRenderer(sdl_renderer);
}

void R_UpdateScreen(){
    SDL_UpdateTexture(screen_texture, NULL, screen_buffer, scrnw * sizeof(unsigned int));
    SDL_RenderTexture(sdl_renderer, screen_texture, NULL, NULL);
    SDL_RenderPresent(sdl_renderer);
}

void R_InitScreen(int w, int h){
    screen_buffer_size = sizeof(unsigned int) * w * h;
    screen_buffer = (unsigned int*)malloc(screen_buffer_size);
    if (screen_buffer == NULL){
        screen_buffer_size = -1;
        printf("Error initializing screen buffer!\n");
        R_Shutdown();
    }

    memset(screen_buffer, 0, screen_buffer_size);

    screen_texture = SDL_CreateTexture(
        sdl_renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        w, h
    );

    if (screen_texture == NULL){
        printf("Error initializing screen texture!\n");
        R_Shutdown();
    }
}

void R_Init(SDL_Window *main_win, game_state_t *game_state){
    window = main_win;
    scrnw = game_state->scrn_w / 2;
    scrnh = game_state->scrn_h / 2;

    sdl_renderer = SDL_CreateRenderer(window, 0);
    R_InitScreen(scrnw, scrnh);
    SDL_SetRenderLogicalPresentation(sdl_renderer, scrnw, scrnh, SDL_LOGICAL_PRESENTATION_STRETCH);
}

void R_Render(player_t *player, game_state_t *game_state){
    is_debug_mode = game_state->is_debug_mode;
    // draw walls
    R_UpdateScreen();
}
