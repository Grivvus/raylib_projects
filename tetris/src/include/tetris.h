#define TITLE "raylib tetris"
#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 600
#define TARGET_FPS 60
#define CUBE_SIDE 60


typedef struct _Tetramino{
    int shape;
    int x;
    int y;
} Tetramino;

typedef struct _GameState{
    int score;
    Tetramino tetramino;
    bool is_game_over;
} GameState;



void gameloop(GameState* gs);
void process(GameState* gs);
void render(GameState* gs);
void handleInput(GameState* gs);
