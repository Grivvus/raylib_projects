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
    unsigned angle;
    bool is_flying;
    Color color;
} tetramino_t;

typedef struct {
    state_t playing_field[FIELD_HEIGHT][FIELD_WIDTH];
    tetramino_t* flying_tetramino;
    unsigned score;
    char* score_str;
} gamestate_t;

typedef struct {
    int x;
    int y;
} pair_t;

/* functions initialization */
void gameloop(gamestate_t* gs);
void init_game_state();
void score_to_str(char* text, unsigned score);
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
void check_full_row(gamestate_t* gs);
void update_highier_squares(gamestate_t* gs, int row_number);
void update_field(gamestate_t* gs);
