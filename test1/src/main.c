#include "raylib.h"

int main(void){
    InitAudioDevice();
    int win_height = 600;
    int win_width = 800;
    InitWindow(win_width, win_height, "main window");

    Sound pop_sound = LoadSound("resources/pop.ogg");
    Texture2D character = LoadTexture("resources/pacman.png");
    float speed_x = 0.01f;
    float speed_y = 0.01f;
    Vector2 pos = {0.0f, 0.0f};
    const float scale = 0.1f;

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (pos.x >= (win_width  - character.width * scale) || pos.x < 0){
            PlaySound(pop_sound);
            speed_x *= (-1);
        }
        if (pos.y >= (win_height - character.height * scale) || pos.y < 0){
            PlaySound(pop_sound);
            speed_y *= (-1);
        }

        pos.x += speed_x;
        pos.y += speed_y;
        DrawTextureEx(character, pos, 0.0f, 0.1f, WHITE);
        EndDrawing();
    }
    return 0;
}
