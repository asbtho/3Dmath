#ifndef R_RENDERER
#define R_RENDERER

#define SDL_MAIN_HANDLED

#include <SDL3/SDL.h>
#include "typedefs.h"
#include "player.h"
#include "g_game_state.h"
#include "utils.h"

typedef struct _r_plane {
    int t[1024];
    int b[1024];
} plane_lut_t;

typedef struct _wall{
    vec2_t a;
    vec2_t b;
    double portal_top_height;
    double portal_bot_height;
    bool is_portal;
} wall_t;

typedef struct _sector {
    int id;
    wall_t walls[10];
    int num_walls;
    int height;
    int elevation;
    double dist;
    unsigned int color;
    unsigned int floor_clr;
    unsigned int ceil_clr;

    plane_lut_t portals_floorx_ylut;
    plane_lut_t portals_ceilx_ylut;
    plane_lut_t floorx_ylut;
    plane_lut_t ceilx_ylut;
} sector_t;

typedef struct _sectors_queue {
    sector_t sectors[1024];
    int num_sectors;
} sectors_queue_t;
   

#endif /* R_RENDERER */
