#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "include/tetris.h"

gamestate_t gamestate = {};

int main(void){
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
        DrawText(gs->score_str, 10, 10, 20, BLACK);
        update_game(gs);
        check_full_row(gs);
        update_field(gs);
        render(gs);
    }
}

void init_game_state(){
    gamestate.flying_tetramino = NULL;
    gamestate.score_str = (char*)malloc(sizeof(char) * 16);
    score_to_str(gamestate.score_str, gamestate.score);
    for (int i = 0; i < FIELD_HEIGHT; i++){
        for (int j = 0; j < FIELD_WIDTH; j++){
            gamestate.playing_field[i][j] = EMPTY;
        }
    }
}

void score_to_str(char* text, unsigned score){
    sprintf(text, "%s %d", "Score: ", score);
}

tetramino_t* get_new_tetramino(){
    tetramino_t* t = (tetramino_t*)malloc(sizeof(tetramino_t));
    t->is_flying = true;
    t->x = SCREEN_WIDTH / 2 - SQUARE_SIDE;
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
            // cube
            t->shape[1][1] = FLY;
            t->shape[1][2] = FLY;
            t->shape[2][1] = FLY;
            t->shape[2][2] = FLY;
            break;
        case 1:
            // I
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
            // Z
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

    if (IsKeyPressed(KEY_W)){
        reverse_generic_tetramino(gs);
    }

    if (IsKeyPressed(KEY_D) && check_side_collisions(gs) != 1){
        t->x += SQUARE_SIDE;
    } else if (IsKeyPressed(KEY_A) && check_side_collisions(gs) != -1){
        t->x -= SQUARE_SIDE;
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
                if (y_cell + 1 >= FIELD_HEIGHT || gs->playing_field[y_cell + 1][x_cell] == FULL){
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
    int x_cell = (gs->flying_tetramino->x / SQUARE_SIDE);
    int y_cell = (gs->flying_tetramino->y / SQUARE_SIDE);

    if (x_cell == 0 || x_cell >= FIELD_WIDTH - 3){
        return false;
    }
    
    x_cell += 1;
    y_cell += 1;
    
    if (gs->playing_field[y_cell][x_cell-1] != EMPTY){
        return false;
    }
    if (gs->playing_field[y_cell+1][x_cell-1] != EMPTY){
        return false;
    }
    if (gs->playing_field[y_cell+1][x_cell+1] != EMPTY){
        return false;
    }
    if (gs->playing_field[y_cell+1][x_cell+2] != EMPTY){
        return false;
    }
    if (gs->playing_field[y_cell+2][x_cell+1] != EMPTY){
        return false;
    }
    if (gs->playing_field[y_cell+2][x_cell+2] != EMPTY){
        return false;
    }
    if (gs->playing_field[y_cell+3][x_cell+1] != EMPTY){
        return false;
    }
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
    /*kinda greedy algo; check 1 extra cell for every rotate;*/
    /*because i don't want check exact cells for every different angle*/
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            int x_cell = (gs->flying_tetramino->x / SQUARE_SIDE) + j;
            int y_cell = (gs->flying_tetramino->y / SQUARE_SIDE) + i;
            if (x_cell < 0 || x_cell >= FIELD_WIDTH){
                return false;
            }
            if (y_cell < 0 || x_cell >= FIELD_HEIGHT){
                return false;
            }
            if (gs->playing_field[y_cell][x_cell] == FULL){
                return false;
            }
        }
    }
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
    /*same algo as for L*/
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            int x_cell = (gs->flying_tetramino->x / SQUARE_SIDE) + j;
            int y_cell = (gs->flying_tetramino->y / SQUARE_SIDE) + i;
            if (x_cell < 0 || x_cell >= FIELD_WIDTH){
                return false;
            }
            if (y_cell < 0 || y_cell >= FIELD_HEIGHT){
                return false;
            }
            if (gs->playing_field[y_cell][x_cell] == FULL){
                return false;
            }
        }
    }
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
    int x_cell = (gs->flying_tetramino->x / SQUARE_SIDE);
    int y_cell = (gs->flying_tetramino->y / SQUARE_SIDE);
    if (gs->playing_field[y_cell][x_cell] != EMPTY){
        return false;
    }
    if (gs->playing_field[y_cell+1][x_cell] != EMPTY){
        return false;
    }
    if (gs->playing_field[y_cell+1][x_cell+3] != EMPTY){
        return false;
    }
    if (gs->playing_field[y_cell+2][x_cell+1] != EMPTY){
        return false;
    }
    if (gs->playing_field[y_cell+2][x_cell+3] != EMPTY){
        return false;
    }
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

void check_full_row(gamestate_t* gs){
    for (int i = 0; i < FIELD_HEIGHT; i++){
        bool is_full_row = true;
        for (int j = 0; j < FIELD_WIDTH; j++){
            if (gs->playing_field[i][j] != FULL){
                is_full_row = false;
                break;
            }
        }
        if (is_full_row){
            update_highier_squares(gs, i);
            for (int x = 0; x < FIELD_WIDTH; x++){
                gs->playing_field[i][x] = EMPTY;
            }
            gs->score += 100;
            score_to_str(gs->score_str, gs->score);
        }
    }
}

void update_highier_squares(gamestate_t* gs, int row_number){
    for (int i = 0; i < row_number; i++){
        for (int j = 0; j < FIELD_WIDTH; j++){
            if (gs->playing_field[i][j] == FULL){
                gs->playing_field[i][j] = FLY;
            }
        }
    }
}

void update_field(gamestate_t* gs){
    for (int i = FIELD_HEIGHT - 1; i >= 0; i--){
        for (int j = 0; j < FIELD_WIDTH; j++){
            if (gs->playing_field[i][j] == FLY){
                if (gs->playing_field[i + 1][j] != FULL){
                    gs->playing_field[i + 1][j] = FULL;
                    gs->playing_field[i][j]  = EMPTY;
                } else {
                    gs->playing_field[i][j] = FULL;
                }
            }
        }
    }
    return;
}
