#include "r_renderer.h"

SDL_Window* window;
SDL_Renderer* sdl_renderer;
SDL_Texture* screen_texture;
unsigned int scrnw, scrnh;

bool is_debug_mode = false;
unsigned int *screen_buffer = NULL;
int screen_buffer_size = 0;
bool debug_text_enabled = true;

sectors_queue_t sectors_queue;

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

void R_UpdateScreen(player_t *player, game_state_t *game_state){
    SDL_RenderClear(sdl_renderer);

    SDL_UpdateTexture(screen_texture, NULL, screen_buffer, scrnw * sizeof(unsigned int));
    SDL_RenderTexture(sdl_renderer, screen_texture, NULL, NULL);

    if (debug_text_enabled){
        SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
        // creates string
        std::ostringstream oss;
        oss << "PLAYER x: " << player->position.x << " y: " << player->position.y << " z: " << player->z << " angle: " << player->dir_angle;
        std::string player_info = oss.str();
        SDL_RenderDebugText(sdl_renderer, 10, 10, player_info.c_str());
        // next string
        //oss.clear();
        //oss.seekp(0);
        //oss << "SECTOR x: " << player->position.x << "y: " << player->position.y << "z: " << player->z;
        //std::string sector_info = oss.str();;
        //SDL_RenderDebugText(sdl_renderer, 10, 30, sector_info.c_str());
        // reset color
        SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
    }

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

void R_DrawPoint(int x, int y, unsigned int color){
    bool is_out_of_bounds = (x < 0 || x > scrnw || y < 0 || y >= scrnh );
    bool is_outside_mem_buff = (scrnw * y + x) >= (scrnw * scrnh);

    if (is_out_of_bounds || is_outside_mem_buff) {
        return;
    }

    screen_buffer[scrnw * y + x] = color;
}

void R_DrawLine(int x0, int y0, int x1, int y1, unsigned int color, player_t *player, game_state_t *game_state){
    int dx;
    if (x1 > x0)
        dx = x1 - x0;
    else
        dx = x0 - x1;

    int dy;
    if (y1 > y0)
        dy = y1 - y0;
    else
        dy = y0 - y1;

    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        R_DrawPoint(x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;

        e2 = err;

        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }

    if (is_debug_mode)
    {
        R_UpdateScreen(player, game_state);
        SDL_Delay(10);
    }
}

void R_ClearScreenBuffer(){
    memset(screen_buffer, 0, sizeof(uint32_t) * scrnw * scrnh);
}

void R_RenderSectors(player_t *player, game_state_t *game_state) {
    // Center point of the screen used to shift NDC (Normalized Device Coordinates) 
    // to screen pixel space (e.g., origin from top-left (0,0)).
    double scrn_half_w = scrnw / 2;
    double scrn_half_h = scrnh / 2;

    // Distance to the projection plane (focal length). 
    // Controls how wide or narrow the view field is.
    double fov = 300;
    unsigned int wall_color = 0xFFFF00FF;

    // Clear previous frame buffer memory before drawing
    R_ClearScreenBuffer();

    // Iterate through all visible sectors queued for rendering
    for (int i = 0; i < sectors_queue.num_sectors; i++) {
        sector_t *s = &sectors_queue.sectors[i];
        int sector_h = s->height;       // Wall vertical extent (Floor-to-Ceiling distance)
        int sector_e = s->elevation;    // Floor elevation off z=0 global ground level
        int sector_clr = s->color;

        // Loop over line segments (walls/portals) bounding the sector
        for (int k = 0; k < s->num_walls; k++) {
            wall_t *w = &s->walls[k];
            
            /* =========================================================================
             * STEP 1: TRANSLATION (World Space -> Camera Relative Space)
             * =========================================================================
             * Shift wall endpoints A (x1, y1) and B (x2, y2) so that the player is 
             * treated as the origin (0, 0).
             *
             *          World Space                   Camera Relative
             *       +-----------------+           +-------------------+
             *       |     Wall (x1,y1)|           |      Wall (dx1,dy1)
             *       |       \         |           |        \          |
             *       |        \        |   ----->  |         \         |
             *       |   Player(P)     |           |       (0,0)[Player]
             *       +-----------------+           +-------------------+
             * ========================================================================= */
            double dx1 = w->a.x - player->position.x;
            double dy1 = w->a.y - player->position.y;
            double dx2 = w->b.x - player->position.x;
            double dy2 = w->b.y - player->position.y;

            /* =========================================================================
             * STEP 2: ROTATION (Camera Space -> View/Screen Alignment Space)
             * =========================================================================
             * Rotate world coordinates by player->dir_angle (theta).
             * Aligns player's facing direction to look straight up the depth axis (Z).
             *
             *  2D Rotation Matrix applied:
             *  [ wx ] = [  sin(θ)  -cos(θ) ] * [ dx ]
             *  [ wz ] = [  cos(θ)   sin(θ) ]   [ dy ]
             *
             *  Resulting Axes:
             *    - wx: Horizontal position relative to player view (Left < 0 < Right)
             *    - wz: Forward distance away from camera view plane (Depth into screen)
             *
             *                  [+wz] (Looking Forward)
             *                    ^
             *                    |
             *                    |    * Wall Point (wx, wz)
             *                    |   /
             *    [-wx] <------- (0,0) [Player] -------> [+wx]
             * ========================================================================= */
            double SN = sin(player->dir_angle);
            double CN = cos(player->dir_angle);
            // Wall Point A transformation
            double wx1 = dx1 * SN - dy1 * CN;
            double wz1 = dx1 * CN + dy1 * SN;
            // Wall Point B transformation
            double wx2 = dx2 * SN - dy2 * CN;
            double wz2 = dx2 * CN + dy2 * SN;

            /* =========================================================================
             * STEP 3: PERSPECTIVE PROJECTION (3D -> 2D Screen Space Scaling)
             * =========================================================================
             * Similar Triangles Principle:
             * Objects further away (larger wz depth) appear smaller on screen.
             * Projection formula: Screen_Length = (World_Length / Depth) * Focal_Length
             *
             *                 View Pyramid / Perspective Triangle:
             *                 
             *                   Projected Screen Wall
             *                          |
             *               +----------|----------+ <-- Real Wall (sector_h)
             *               |          |          |
             *     Player (0,0) ------>[fov]------>[wz1] (Depth)
             *               |          |          |
             *               +----------|----------+
             *                          |<--wh1--->|
             * ========================================================================= */
            // Calculate apparent vertical height on screen for wall endpoints A and B
            double wh1 = (sector_h / wz1) * fov;
            double wh2 = (sector_h / wz2) * fov;
            
            /*
             * Map 2D transformed coordinates (wx, wz) to 2D screen coordinate offsets:
             *  - sx: Horizontal pixel offset from screen center
             *  - sy: Vertical baseline offset (accounting for player eye height & Z elevation)
             */
            double sx1 = (wx1 / wz1) * fov;
            double sy1 = ((game_state->scrn_h + player->z) / wz1);
            double sx2 = (wx2 / wz2) * fov;
            double sy2 = ((game_state->scrn_h + player->z) / wz2);

            /* =========================================================================
             * STEP 4: SECTOR ELEVATION ADJUSTMENT
             * =========================================================================
             * Adjust base Y position on screen to reflect sector step-ups/drops 
             * (floor height offsets relative to ground level z=0).
             * Subtracted because screen Y increases downward (+Y = down).
             * ========================================================================= */
            double s_level1 = (sector_e / wz1) * fov;
            double s_level2 = (sector_e / wz2) * fov;
            sy1 -= s_level1;
            sy2 -= s_level2;

            /* =========================================================================
             * STEP 5: PORTAL BOUNDARIES (Optional Opening Traversal)
             * =========================================================================
             * If this wall is a portal connecting two sectors, project top and bottom 
             * beam steps (doorways, ledges, windows).
             * ========================================================================= */
            double pbh1 = 0;
            double pbh2 = 0;
            double pth1 = 0;
            double pth2 = 0;
            if ( w->is_portal ) {
                pth1 = (w->portal_top_height / wz1) * fov;
                pth2 = (w->portal_top_height / wz2) * fov;
                pbh1 = (w->portal_bot_height / wz1) * fov;
                pbh2 = (w->portal_bot_height / wz2) * fov;
            }

            /* =========================================================================
             * STEP 6: SCREEN CENTER OFFSET SHIFT
             * =========================================================================
             * Convert centered space (0,0 at screen center) to standard window space
             * where (0,0) sits at top-left.
             *
             *  (-w/2, -h/2)   (0, -h/2)   (+w/2, -h/2)       (0,0) ------------ (scrnw,0)
             *              \      |      /                   |                     |
             *  (-w/2, 0) ----- (0,0) ----- (+w/2, 0)  ===>   |     (scrn_half)     |
             *              /      |      \                   |                     |
             *  (-w/2, +h/2)   (0, +h/2)   (+w/2, +h/2)       (0,scrnh) -------- (scrnw,scrnh)
             * ========================================================================= */
            sx1 += scrn_half_w;
            sy1 += scrn_half_h;
            sx2 += scrn_half_w;
            sy2 += scrn_half_h;

            /* =========================================================================
             * STEP 7: RASTERIZATION / LINE DRAWING
             * =========================================================================
             * Connect screen projected vertices to form the wireframe box/quad of wall.
             *
             *   (sx1, sy1 - wh1)  -----------------------  (sx2, sy2 - wh2)  <-- Top Line
             *          |                                          |
             *          | Left Edge                                | Right Edge
             *          |                                          |
             *     (sx1, sy1)      -----------------------     (sx2, sy2)     <-- Bottom Line
             * ========================================================================= */
            // Top ceiling edge line
            R_DrawLine(sx1, sy1 - wh1, sx2, sy2 - wh2, wall_color, player, game_state);
            // Bottom floor edge line
            R_DrawLine(sx1, sy1, sx2, sy2, wall_color, player, game_state);
            // Left vertical wall border
            R_DrawLine(sx1, sy1 - wh1, sx1, sy1, wall_color, player, game_state);
            // Right vertical wall border
            R_DrawLine(sx2, sy2 - wh2, sx2, sy2, wall_color, player, game_state);
        }
    }

    // Render top-down overlay on top of frame
    R_DrawDebugMinimap(player, game_state);
}

void R_DrawDebugMinimap(player_t *player, game_state_t *game_state) {
    int map_x = scrnw - 170;
    double scrn_half_w = scrnw / 2;
    double fov = 300;
    int map_y = 10;
    int map_size = 150;
    double map_scale = 0.75;

    int center_x = map_x + map_size / 2;
    int center_y = map_y + map_size / 2;

    uint32_t COLOR_BG       = 0xCC000000; 
    uint32_t COLOR_MAP_LINE = 0xFFFFFFFF; 
    uint32_t COLOR_PLAYER   = 0xFFFF0000; 
    uint32_t COLOR_CAM_WALL = 0xFF00FFFF; 
    uint32_t COLOR_FOV      = 0x88FFFFFF; 

    // Draw background boundary box
    for (int y = map_y; y < map_y + map_size; y++) {
        for (int x = map_x; x < map_x + map_size; x++) {
            R_DrawPoint(x, y, COLOR_BG);
            if (x == map_x || x == map_x + map_size-1 || y == map_y || y == map_y + map_size-1) {
                R_DrawPoint(x, y, COLOR_MAP_LINE);
            } 
        }
    }

    // Player Dot at Minimap Center
    R_DrawLine(center_x - 2, center_y, center_x + 2, center_y, COLOR_PLAYER, player, game_state);
    R_DrawLine(center_x, center_y - 2, center_x, center_y + 2, COLOR_PLAYER, player, game_state);

    //Player Facing Vector
    R_DrawLine(center_x, center_y, center_x, center_y - 15, COLOR_PLAYER, player, game_state);

    // Draw FOV Lines (Perspective bounds)
    // FOV half-angle calculated from focal length: angle = atan((scrnw / 2) / fov)
    double half_fov_rad = atan(scrn_half_w / fov);
    int fov_len = 50;
    
    int fov_left_x  = center_x - (int)(sin(half_fov_rad) * fov_len);
    int fov_left_y  = center_y - (int)(cos(half_fov_rad) * fov_len);
    int fov_right_x = center_x + (int)(sin(half_fov_rad) * fov_len);
    int fov_right_y = center_y - (int)(cos(half_fov_rad) * fov_len);

    R_DrawLine(center_x, center_y, fov_left_x, fov_left_y, COLOR_FOV, player, game_state);
    R_DrawLine(center_x, center_y, fov_right_x, fov_right_y, COLOR_FOV, player, game_state);

    double SN = sin(player->dir_angle);
    double CN = cos(player->dir_angle);

    for (int i = 0; i < sectors_queue.num_sectors; i++) {
        sector_t *s = &sectors_queue.sectors[i];
        for (int k = 0; k < s->num_walls; k++) {
            wall_t *w = &s->walls[k];

            double dx1 = w->a.x - player->position.x;
            double dy1 = w->a.y - player->position.y;
            double dx2 = w->b.x - player->position.x;
            double dy2 = w->b.y - player->position.y;

            double wx1 = dx1 * SN - dy1 * CN;
            double wz1 = dx1 * CN + dy1 * SN;
            double wx2 = dx2 * SN - dy2 * CN;
            double wz2 = dx2 * CN + dy2 * SN;

            int map_wx1 = center_x + (int)(wx1 * map_scale);
            int map_wz1 = center_y - (int)(wz1 * map_scale); 
            int map_wx2 = center_x + (int)(wx2 * map_scale);
            int map_wz2 = center_y - (int)(wz2 * map_scale);

            R_DrawLine(map_wx1, map_wz1, map_wx2, map_wz2, COLOR_CAM_WALL, player, game_state);
        }
    }
}

void R_Render(player_t *player, game_state_t *game_state){
    is_debug_mode = game_state->is_debug_mode;

    R_RenderSectors(player, game_state);
    R_UpdateScreen(player, game_state);
}

sector_t R_CreateSector(int height, int elevation, unsigned int color, unsigned int ceil_clr, unsigned int floor_clr){
    static int sector_id = 0;
    sector_t sector = {0};
    sector.num_walls = 0;
    sector.height = height;
    sector.elevation = elevation;
    sector.color = color;
    sector.ceil_clr = ceil_clr;
    sector.floor_clr = floor_clr;
    sector.id = ++sector_id;
    return sector;
}

void R_SectorAddWall(sector_t *sector, wall_t vertices){
    sector->walls[sector->num_walls] = vertices;
    sector->num_walls++;
}

void R_AddSectorToQueue(sector_t *sector){
    sectors_queue.sectors[sectors_queue.num_sectors] = *sector;
    sectors_queue.num_sectors++;
}

wall_t R_CreateWall(int ax, int ay, int bx, int by){
    wall_t w;
    w.a.x = ax;
    w.a.y = ay;
    w.b.x = bx;
    w.b.y = by;
    w.is_portal = false;

    return w;
}
