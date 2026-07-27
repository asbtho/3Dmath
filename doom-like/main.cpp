#include <stdio.h>

#include "p_player.h"
#include "g_game_state.h"
#include "w_window.h"
#include "r_renderer.h"
#include "k_keyboard.h"

#define SCRNW 1024
#define SCRNH 768
#define FPS 120

void GameLoop(game_state_t *game_state, player_t *player) {
    while(game_state->is_running){
        G_FrameStart();

        K_HandleEvents(game_state, player);
        R_Render(player, game_state);

        G_FrameEnd(game_state);
    }
}

int main() {
    game_state_t game_state = G_Init(SCRNW, SCRNH, FPS);
    player_t player = P_Init(40, 40, SCRNH * 10, M_PI / 2);
    K_InitKeymap();
    W_Init(SCRNW, SCRNH);
    R_Init(W_Get(), &game_state);

    GameLoop(&game_state, &player);

    return 0;
}
