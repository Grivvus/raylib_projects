#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "raylib.h"

/* preprocessors defines */
#define TARGET_FPS 60
#define SQUARE_SIDE 40 // длина/ширина стороны квадрата в пикселях
#define FIELD_HEIGHT 20 // высота игрового поля в квадратах
#define FIELD_WIDTH 12 // ширина игрового поля в квадратах

#define SCREEN_HEIGHT (FIELD_HEIGHT * SQUARE_SIDE)
#define SCREEN_WIDTH (FIELD_WIDTH * SQUARE_SIDE)

/* custom types */
typedef enum {
    EMPTY, FULL, FLY
} state_t;

typedef struct {
    int shape_n;
    state_t shape[4][4];
    int x;
    int y;
    uint16_t angle;
    bool is_flying;
    Color color;
} tetramino_t;

typedef struct {
    state_t playing_field[FIELD_HEIGHT][FIELD_WIDTH];
    tetramino_t* flying_tetramino;
    int score;
} gamestate_t;

typedef struct {
    int x;
    int y;
} pair_t;

/* functions initialization */
void gameloop(gamestate_t* gs);
void init_game_state();
tetramino_t* get_new_tetramino();
void set_tetramino_shape(tetramino_t* t, int shape);
void update_game(gamestate_t* gs);
void update_tetramino_pos(gamestate_t* gs);
int check_side_collisions(gamestate_t* gs);
bool check_bottom_collision(gamestate_t* gs);
void reverse_generic_tetramino(gamestate_t* gs);
void reverse_I_tetramino(gamestate_t* gs);
bool check_I_tetramino_reversable(gamestate_t* gs);
void reverse_L_tetramino(gamestate_t* gs);
bool check_L_tetramino_reversable(gamestate_t* gs);
void reverse_T_tetramino(gamestate_t* gs);
bool check_T_tetramino_reversable(gamestate_t* gs);
void reverse_Z_tetramino(gamestate_t* gs);
bool check_Z_tetramino_reversable(gamestate_t* gs);
void render(gamestate_t* gs);
void render_tetramino(tetramino_t* t);
void render_field(gamestate_t* gs);
Color get_rand_color();


gamestate_t gamestate = {};

int main(int argc, char** argv){
    srand(time(NULL));
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tetris");
    SetTargetFPS(TARGET_FPS);
    init_game_state();

    tetramino_t* t = get_new_tetramino();
    gamestate.flying_tetramino = t;

    gameloop(&gamestate);

    return 0;
}

void gameloop(gamestate_t* gs){
    int cnt = 0;
    while (WindowShouldClose() == false){
        /*if (cnt == 3){*/
        /*    update_game(gs);*/
        /*    cnt = 0;*/
        /*} else {*/
        /*    cnt++;*/
        /*}*/
        update_game(gs);
        render(gs);
    }
}

void init_game_state(){
    gamestate.score = 0;
    gamestate.flying_tetramino = NULL;
    for (int i = 0; i < FIELD_HEIGHT; i++){
        for (int j = 0; j < FIELD_WIDTH; j++){
            gamestate.playing_field[i][j] = EMPTY;
        }
    }
}

tetramino_t* get_new_tetramino(){
    tetramino_t* t = (tetramino_t*)malloc(sizeof(tetramino_t));
    t->is_flying = true;
    t->x = (SCREEN_WIDTH - 4 * SQUARE_SIDE) / 3;
    t->y = 0;
    t->color = get_rand_color();
    t->angle = 0;
    set_tetramino_shape(t, rand() % 5);

    return t;
}

void set_tetramino_shape(tetramino_t* t, int shape){
    t->shape_n = shape;
    t->angle = 0;

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            t->shape[i][j] = EMPTY;
        }
    }

    switch (shape) {
        case 0:
            // квадрат
            t->shape[1][1] = FLY;
            t->shape[1][2] = FLY;
            t->shape[2][1] = FLY;
            t->shape[2][2] = FLY;
            break;
        case 1:
            // палка вертикальная
            t->shape[0][1] = FLY;
            t->shape[1][1] = FLY;
            t->shape[2][1] = FLY;
            t->shape[3][1] = FLY;
            break;
        case 2:
            // L
            t->shape[0][1] = FLY;
            t->shape[1][1] = FLY;
            t->shape[2][1] = FLY;
            t->shape[2][2] = FLY;
            break;
        case 3:
            // T
            t->shape[1][0] = FLY;
            t->shape[1][1] = FLY;
            t->shape[1][2] = FLY;
            t->shape[2][1] = FLY;
            break;
        case 4:
            // молния/лесенка
            t->shape[0][1] = FLY;
            t->shape[1][1] = FLY;
            t->shape[1][2] = FLY;
            t->shape[2][2] = FLY;
            break;
    }
}

void update_game(gamestate_t* gs){
    if (check_bottom_collision(gs) == true){
        // еще летим
        update_tetramino_pos(gs);
    } else {
        // уже не летим
        gs->flying_tetramino->is_flying = false;
        for (int i = 0; i < 4; i++){
            for (int j = 0; j < 4; j++){
                if (gs->flying_tetramino->shape[i][j] == FLY){
                    int x_cell, y_cell;
                    x_cell = (gs->flying_tetramino->x / SQUARE_SIDE) + j;
                    y_cell = (gs->flying_tetramino->y / SQUARE_SIDE) + i;
                    gs->playing_field[y_cell][x_cell] = FULL;
                }
            }
        }
        gs->flying_tetramino = get_new_tetramino();
    }
}

void update_tetramino_pos(gamestate_t* gs){
    // если зашли в эту функцию, значит фигура еще точно летит
    tetramino_t* t = gs->flying_tetramino;
    t->y += (SQUARE_SIDE / (TARGET_FPS / 4));

    if (IsKeyDown(KEY_D) && IsKeyDown(KEY_A)){
        return;
    }

    if (IsKeyPressed(KEY_R)){
        reverse_generic_tetramino(gs);
    }

    if (IsKeyDown(KEY_D) && check_side_collisions(gs) != 1){
        t->x += (SQUARE_SIDE / 2);
    } else if (IsKeyDown(KEY_A) && check_side_collisions(gs) != -1){
        t->x -= (SQUARE_SIDE / 2);
    }
}

int check_side_collisions(gamestate_t* gs){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            if (gs->flying_tetramino->shape[i][j] == FLY){
                int x_cell, y_cell;
                x_cell = (gs->flying_tetramino->x / SQUARE_SIDE) + j;
                y_cell = (gs->flying_tetramino->y / SQUARE_SIDE) + i;
                // левая коллизия очень странная и некорректная
                if (x_cell - 1 < 0 || gs->playing_field[y_cell][x_cell - 1] == FULL){
                    return -1;
                }
                if (x_cell + 1 >= FIELD_WIDTH || gs->playing_field[y_cell][x_cell + 1] == FULL){
                    return 1;
                }
            }
        }
    }
    return 0;
}

bool check_bottom_collision(gamestate_t* gs){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            if (gs->flying_tetramino->shape[i][j] == FLY){
                int x_cell, y_cell;
                x_cell = (gs->flying_tetramino->x / SQUARE_SIDE) + j;
                y_cell = (gs->flying_tetramino->y / SQUARE_SIDE) + i;
                if (y_cell + 1 >= FIELD_HEIGHT || gs->playing_field[y_cell][x_cell] == FULL){
                    return false;
                }
            }
        }
    }
    return true;
}

void reverse_generic_tetramino(gamestate_t* gs){
    switch(gs->flying_tetramino->shape_n){
        case 0: return; break;
        case 1: reverse_I_tetramino(gs); break;
        case 2: reverse_L_tetramino(gs); break;
        case 3: reverse_T_tetramino(gs); break;
        case 4: reverse_Z_tetramino(gs); break;
    }
}

void reverse_I_tetramino(gamestate_t* gs){
    if (check_I_tetramino_reversable(gs) == false)
        return;

    // палка может ворочиться только на 90 градусов
    if (gs->flying_tetramino->angle == 90){
        set_tetramino_shape(gs->flying_tetramino, 1);
        return;
    }

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            gs->flying_tetramino->shape[i][j] = EMPTY;
        }
    }

    gs->flying_tetramino->angle = 90;
    gs->flying_tetramino->shape[1][0] = FLY;
    gs->flying_tetramino->shape[1][1] = FLY;
    gs->flying_tetramino->shape[1][2] = FLY;
    gs->flying_tetramino->shape[1][3] = FLY;
}

bool check_I_tetramino_reversable(gamestate_t* gs){
    return true;
}

void reverse_L_tetramino(gamestate_t* gs){
    if (check_L_tetramino_reversable(gs) == false)
        return;

    if (gs->flying_tetramino->angle == 270){
        set_tetramino_shape(gs->flying_tetramino, 2);
        return;
    }
    
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            gs->flying_tetramino->shape[i][j] = EMPTY;
        }
    }

    if (gs->flying_tetramino->angle == 0){
        gs->flying_tetramino->shape[0][2] = FLY;
        gs->flying_tetramino->shape[1][0] = FLY;
        gs->flying_tetramino->shape[1][1] = FLY;
        gs->flying_tetramino->shape[1][2] = FLY;
    } else if (gs->flying_tetramino->angle == 90){
        gs->flying_tetramino->shape[0][0] = FLY;
        gs->flying_tetramino->shape[0][1] = FLY;
        gs->flying_tetramino->shape[1][1] = FLY;
        gs->flying_tetramino->shape[2][1] = FLY;
    } else if (gs->flying_tetramino->angle == 180){
        gs->flying_tetramino->shape[1][0] = FLY;
        gs->flying_tetramino->shape[1][1] = FLY;
        gs->flying_tetramino->shape[1][2] = FLY;
        gs->flying_tetramino->shape[2][0] = FLY;
    }

    gs->flying_tetramino->angle += 90;
    return;
}

bool check_L_tetramino_reversable(gamestate_t* gs){
    return true;
}

void reverse_T_tetramino(gamestate_t* gs){
     if (check_T_tetramino_reversable(gs) == false)
        return;

    if (gs->flying_tetramino->angle == 270){
        set_tetramino_shape(gs->flying_tetramino, 3);
        return;
    }
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            gs->flying_tetramino->shape[i][j] = EMPTY;
        }
    }
    if (gs->flying_tetramino->angle == 0){
        gs->flying_tetramino->shape[0][1] = FLY;
        gs->flying_tetramino->shape[1][1] = FLY;
        gs->flying_tetramino->shape[1][2] = FLY;
        gs->flying_tetramino->shape[2][1] = FLY;
    } else if (gs->flying_tetramino->angle == 90){
        gs->flying_tetramino->shape[0][1] = FLY;
        gs->flying_tetramino->shape[1][0] = FLY;
        gs->flying_tetramino->shape[1][1] = FLY;
        gs->flying_tetramino->shape[1][2] = FLY;
    } else if (gs->flying_tetramino->angle == 180){
        gs->flying_tetramino->shape[0][1] = FLY;
        gs->flying_tetramino->shape[1][0] = FLY;
        gs->flying_tetramino->shape[1][1] = FLY;
        gs->flying_tetramino->shape[2][1] = FLY;
    }

    gs->flying_tetramino->angle += 90;
    return;
}

bool check_T_tetramino_reversable(gamestate_t* gs){
    return true;
}

void reverse_Z_tetramino(gamestate_t* gs){
    if (check_Z_tetramino_reversable(gs) == false)
        return;

    if (gs->flying_tetramino->angle == 90){
        set_tetramino_shape(gs->flying_tetramino, 4);
        return;
    }

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            gs->flying_tetramino->shape[i][j] = EMPTY;
        }
    }

    gs->flying_tetramino->angle = 90;
    gs->flying_tetramino->shape[2][1] = FLY;
    gs->flying_tetramino->shape[2][2] = FLY;
    gs->flying_tetramino->shape[1][2] = FLY;
    gs->flying_tetramino->shape[1][3] = FLY;
    return;
}

bool check_Z_tetramino_reversable(gamestate_t* gs){
    return true;
}

void render(gamestate_t* gs){
    ClearBackground(WHITE);

    render_tetramino(gs->flying_tetramino);
    render_field(gs);

    EndDrawing();
}

void render_tetramino(tetramino_t* t){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            if (t->shape[i][j] == FLY){
                /*printf("[info] rendering tetramino with shape %d\n", t->shape_n);*/
                /*printf("[info] i=%d, j=%d\n", i, j);*/
                DrawRectangle(t->x + SQUARE_SIDE * j, t->y + SQUARE_SIDE * i, SQUARE_SIDE, SQUARE_SIDE, t->color);
            }
        }
    }
}

void render_field(gamestate_t* gs){
    for (int i = 0; i < FIELD_HEIGHT; i++){
        for (int j = 0; j < FIELD_WIDTH; j++){
            if (gs->playing_field[i][j] == FULL){
                DrawRectangle(SQUARE_SIDE * j, SQUARE_SIDE * i, SQUARE_SIDE, SQUARE_SIDE, PURPLE);
            }
        }
    }
}

Color get_rand_color(){
    int i = rand() % 5;
    switch (i) {
        case 0: return YELLOW; break;
        case 1: return BLUE; break;
        case 2: return PURPLE; break;
        case 3: return RED; break;
        case 4: return GREEN; break;
        default: return WHITE; break;
    }
}
