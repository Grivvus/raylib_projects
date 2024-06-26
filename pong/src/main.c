#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "raylib.h"
#include "include/constants.h"
#include "include/pong.h"

// global parameters
int ball_x = WINDOW_WIDTH / 2;
int ball_y = WINDOW_HEIGHT / 2;
int ball_speed_x = 5;
int ball_speed_y;
int my_platform_x_pos = 0;
int my_platform_y_pos = WINDOW_HEIGHT / 2;
int enemy_platform_x_pos = WINDOW_WIDTH - PLATFORM_WIDTH;
int enemy_platform_y_pos = WINDOW_HEIGHT / 2;
char* score_text;
unsigned int score = 0;


int main(int argc, char* argv[]){
    srand(time(NULL));
    score_text = (char*)malloc(sizeof(char) * 16);
    ball_speed_y = (rand() % 20) - 10;
    while (ball_speed_y == 0){
        ball_speed_y = (rand() % 20) - 10;
    }
    SetTargetFPS(TARGET_FPS);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, TITLE);
    gameloop();
    return 0;
}


void gameloop(){
    while(WindowShouldClose() == false){
        handleInput();
        process();
        render();
    }
}

void handleInput(){
    if (IsKeyDown(KEY_W)){
        change_my_platform_pos('u');
    }
    else if (IsKeyDown(KEY_S)){
        change_my_platform_pos('d');
    }
    return;
}

void process(){
    handle_ball_collisions();
    change_ball_position();
    enemy_logic();
    check_for_loose();
}

void render(){
    update_score_text();
    DrawText(score_text, 10, 10, 20, BLACK);
    BeginDrawing();
    ClearBackground(WHITE);
    DrawCircle(ball_x, ball_y, BALL_RADIUS, BLUE);
    DrawRectangle(my_platform_x_pos, my_platform_y_pos, PLATFORM_WIDTH, PLATFORM_HEIGHT, BLACK);
    DrawRectangle(enemy_platform_x_pos, enemy_platform_y_pos, PLATFORM_WIDTH, PLATFORM_HEIGHT, BLACK);

    EndDrawing();
    return;
}

void update_score_text(){
    sprintf(score_text, "%s %d", "Score:", score);
}

void handle_ball_collisions(){
    if (ball_y <= 0 || ball_y >= WINDOW_HEIGHT){
        ball_speed_y *= -1;
    }
    if ((ball_x <= PLATFORM_WIDTH && (ball_y >= my_platform_y_pos && ball_y <= my_platform_y_pos + PLATFORM_HEIGHT)) || ((ball_x >= (WINDOW_WIDTH - PLATFORM_WIDTH) && (ball_y >= enemy_platform_y_pos && ball_y <= enemy_platform_y_pos + PLATFORM_HEIGHT)))){
        score += 1;
        speedup_ball();
        ball_speed_x *= -1;
    }
    return;
}

void change_ball_position(){
    ball_x += ball_speed_x;
    ball_y += ball_speed_y;
    return;
}

void change_my_platform_pos(char direction){
    if (direction == 'u'){
        if (my_platform_y_pos <= 0)
            return;
        my_platform_y_pos -= PLATFORM_SPEED;
    }
    else if (direction == 'd'){
        if (my_platform_y_pos + PLATFORM_HEIGHT > WINDOW_HEIGHT)
            return;
        my_platform_y_pos += PLATFORM_SPEED;
    }
}

void check_for_loose(){
    if (ball_x < 0 || ball_x > WINDOW_WIDTH){
        CloseWindow();
    }
}

void enemy_logic(){
    if (ball_speed_x > 0){
        int y_pos = calc_final_y_pos();
        if (enemy_platform_y_pos > y_pos){
            enemy_platform_y_pos -= PLATFORM_SPEED;
        }
        else if (enemy_platform_y_pos + PLATFORM_HEIGHT < y_pos){
            enemy_platform_y_pos += PLATFORM_SPEED;
        }
    }
}

int calc_final_y_pos(){
    unsigned int game_cycles = (enemy_platform_x_pos - ball_x) / ball_speed_x;
    int y_pos = ball_y + (ball_speed_y * game_cycles);
    while (y_pos > WINDOW_HEIGHT || y_pos < 0){
        if (y_pos > WINDOW_HEIGHT){
            y_pos = WINDOW_HEIGHT - (y_pos - WINDOW_HEIGHT);
        }
        else if (y_pos < 0){
            y_pos *= -1;
        }
    }
    return y_pos;
}

void speedup_ball(){
    if (score % 10 == 0){
        if (ball_speed_x > 0)
            ball_speed_x += 2;
        else
            ball_speed_x -= 2;
    }
}
