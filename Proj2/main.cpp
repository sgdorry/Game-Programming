/**
* Author: Steven Dorry
* Assignment: Pong Clone
* Date due: 02/28/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "/opt/homebrew/opt/raylib/include/raylib.h"
#include <cmath>

// Global Constants
constexpr int SCREEN_WIDTH  =   800,
              SCREEN_HEIGHT =   500,
              FPS           =   60,
              WIN_SCORE     =   4,
              MAX_BALLS     =   3;

constexpr float PADDLE_SPEED = 350.0f,
                BALL_SPEED   = 250.0f;

constexpr Color RULE_COLOR   = {242, 198, 203, 255};

struct Paddle{
    Vector2 size;
    Vector2 position; 
};

struct Ball {
    Vector2 size;
    Vector2 position;
    Vector2 velocity;
    bool isActive; 
};

Paddle    gLeftPaddle, 
          gRightPaddle;

Texture2D gPaddleTexture,
          gBallTexture,
          gBackgroundTexture;

Ball      gBalls[MAX_BALLS];

int  gLeftScore    = 0,
     gRightScore   = 0,
     gBallCount    = 1;
bool gSinglePlayer = false,
     gGameOver     = false;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, 
                 const Vector2 *positionB, const Vector2 *scaleB){
    float xDistance = fabs(positionA->x- positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y) /2.0f);
    if (xDistance < 0.0f && yDistance < 0.0f){
        return true;
    }
   
    return false;
}

void renderObject(const Texture2D *texture, const Vector2 *position, const Vector2 *size){
    Rectangle textureArea = {
        0.0f, 0.0f,
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    Rectangle destinationArea = {
        position->x, position->y,
        size->x, size->y
    };

    Vector2 originOffset = {
        size->x / 2.0f,
        size->y / 2.0f
    };

    DrawTexturePro(*texture, textureArea, destinationArea, originOffset, 0.0f, WHITE);
}

void resetBall(Ball *ball, bool launchRight){
    ball->position = {(float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT / 2.0f};
    float ySign = (GetRandomValue(0, 1) == 0) ? -1.0f : 1.0f;
    float yMagnitude = (float)GetRandomValue(100, 200);
    ball->velocity = {launchRight ? BALL_SPEED : -BALL_SPEED, ySign * yMagnitude};
}


void configureBallCount(int count){
    gBallCount = count;
    for (int i = 0; i <MAX_BALLS; i++)
    {
        gBalls[i].isActive = (i < gBallCount);
        // flip direction
        if (gBalls[i].isActive) {
            resetBall(&gBalls[i], (i % 2) == 0);
        }
    }
}

void resetMatch(){
    gLeftPaddle.position = {50.0f, (float)SCREEN_HEIGHT / 2.0f};
    gLeftPaddle.size = {20.0f, 120.0f};

    gRightPaddle.position = {(float)SCREEN_WIDTH - 50.0f, (float)SCREEN_HEIGHT / 2.0f};
    gRightPaddle.size = {20.0f, 120.0f};

    gLeftScore = 0;
    gRightScore = 0;
    gGameOver = false;

    for (int i = 0; i < MAX_BALLS; i++)
    {
        gBalls[i].size = {25.0f, 25.0f};
        gBalls[i].isActive= false;
    }
    configureBallCount(gBallCount);
}

void initialise(){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong!");

    gPaddleTexture=LoadTexture("paddle.png");
    gBallTexture=LoadTexture("ball.png");
    gBackgroundTexture=LoadTexture("background.png");

    SetTargetFPS(FPS);

    resetMatch();
}

void processInput(){
    if (IsKeyPressed(KEY_T)){
        gSinglePlayer = !gSinglePlayer;
    }

    if (IsKeyPressed(KEY_ONE)){
        configureBallCount(1);
    }
    else if (IsKeyPressed(KEY_TWO)){
        configureBallCount(2);
    }
    else if (IsKeyPressed(KEY_THREE)){
        configureBallCount(3);
    }
    if (gGameOver && IsKeyPressed(KEY_R)) {
        resetMatch();
    }
}


void updatePaddles(float deltaTime){
    if (IsKeyDown(KEY_W)) {
        gLeftPaddle.position.y -= PADDLE_SPEED * deltaTime;
    }
    if (IsKeyDown(KEY_S)){
        gLeftPaddle.position.y += PADDLE_SPEED * deltaTime;
    }

    if (!gSinglePlayer){
        if (IsKeyDown(KEY_UP)){
            gRightPaddle.position.y -= PADDLE_SPEED * deltaTime;
        }
        if (IsKeyDown(KEY_DOWN)){
            gRightPaddle.position.y += PADDLE_SPEED * deltaTime;
        }
    }
    else{
        for (int i = 0; i < MAX_BALLS; i++){
            if (!gBalls[i].isActive){
                continue;
            }
            if (gBalls[i].position.y < gRightPaddle.position.y - 10.0f)
                gRightPaddle.position.y -= PADDLE_SPEED * deltaTime;
            else if (gBalls[i].position.y > gRightPaddle.position.y + 10.0f)
                gRightPaddle.position.y += PADDLE_SPEED * deltaTime;
            break;
        }
    }

    if (gLeftPaddle.position.y - gLeftPaddle.size.y / 2.0f < 0.0f){
        gLeftPaddle.position.y = gLeftPaddle.size.y / 2.0f;
    }
    if (gLeftPaddle.position.y + gLeftPaddle.size.y / 2.0f > (float)SCREEN_HEIGHT){
        gLeftPaddle.position.y = (float)SCREEN_HEIGHT - gLeftPaddle.size.y / 2.0f;
    }

    if (gRightPaddle.position.y - gRightPaddle.size.y / 2.0f < 0.0f){
        gRightPaddle.position.y = gRightPaddle.size.y / 2.0f;
    }
    if (gRightPaddle.position.y + gRightPaddle.size.y / 2.0f > (float)SCREEN_HEIGHT){
        gRightPaddle.position.y = (float)SCREEN_HEIGHT - gRightPaddle.size.y / 2.0f;
    }
}

void updateBalls(float deltaTime){
    for (int i = 0; i < MAX_BALLS; i++){
        if (!gBalls[i].isActive){
            continue;
        }

        Ball* ball = &gBalls[i];
        ball->position.x += ball->velocity.x * deltaTime;
        ball->position.y += ball->velocity.y * deltaTime;

        float halfH = ((ball->size.y) / 2.0f);
        if (ball->position.y - halfH <= 0.0f){
            ball->position.y = halfH;
            ball->velocity.y *= -1.0f;}
        if (ball->position.y + halfH >= (float)SCREEN_HEIGHT){
            ball->position.y = (float)SCREEN_HEIGHT - halfH;
            ball->velocity.y *= -1.0f;
        }

        if (isColliding(&ball->position, &ball->size, &gLeftPaddle.position, &gLeftPaddle.size) && ball->velocity.x < 0.0f){
            // push ball
            ball->position.x = gLeftPaddle.position.x + (gLeftPaddle.size.x + ball->size.x) / 2.0f;
            ball->velocity.x *= -1.0f;
        }

        if (isColliding(&ball->position, &ball->size, &gRightPaddle.position, &gRightPaddle.size) && ball->velocity.x > 0.0f){
            ball->position.x = gRightPaddle.position.x - (gRightPaddle.size.x + ball->size.x) / 2.0f;
            ball->velocity.x *= -1.0f;
        }

        float halfW = ball->size.x / 2.0f;
        if (ball->position.x + halfW < 0.0f){
            gRightScore++;
            resetBall(ball,true);
        }
        else if (ball->position.x - halfW > (float)SCREEN_WIDTH){
            gLeftScore++;
            resetBall(ball, false);
        }
    }

    if (gLeftScore >= WIN_SCORE || gRightScore >= WIN_SCORE){
        gGameOver = true;
    }
}

void update(){
    float deltaTime = GetFrameTime();
    if (gGameOver){
        return;
    }

    updatePaddles(deltaTime);
    updateBalls(deltaTime);}

void render(){
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(gBackgroundTexture, 0, 0, WHITE);

    renderObject(&gPaddleTexture, &gLeftPaddle.position,&gLeftPaddle.size);
    renderObject(&gPaddleTexture, &gRightPaddle.position, &gRightPaddle.size);

    for (int i = 0; i<MAX_BALLS; i++){
        if (gBalls[i].isActive) {
            renderObject(&gBallTexture, &gBalls[i].position, &gBalls[i].size);
        }
    }

    DrawText(TextFormat("Left: %d", gLeftScore), 20, 20, 24, RULE_COLOR);
    DrawText(TextFormat("Right: %d", gRightScore), SCREEN_WIDTH - 150, 20, 24, RULE_COLOR);
    const char* modeText = TextFormat("Mode: %s (T to change mode)", gSinglePlayer ? "1-Player" : "2-Player");
    DrawText(modeText, SCREEN_WIDTH / 2 - MeasureText(modeText, 20) / 2, 20, 20, RULE_COLOR);
    const char* ballText = TextFormat("Balls: %d (Press 1/2/3 to change #balls)", gBallCount);
    DrawText(ballText, SCREEN_WIDTH / 2 - MeasureText(ballText, 20) / 2, 45, 20, RULE_COLOR);

    if (gGameOver){
        const char* winner = (gLeftScore > gRightScore) ? "Left Player Wins" : "Right Player Wins";
        DrawText(winner, 300, SCREEN_HEIGHT / 2 - 20, 36, ORANGE);
        DrawText("Game Over ... Press R to restart", 250, SCREEN_HEIGHT / 2 + 25, 24, RULE_COLOR);
    }

    EndDrawing();
}

void shutdown(){
    UnloadTexture(gPaddleTexture);
    UnloadTexture(gBallTexture);
    UnloadTexture(gBackgroundTexture);

    CloseWindow();

}

int main(){
    initialise();
    while (!WindowShouldClose()){
        processInput();
        update();
        render();
    }
    shutdown();
    return 0;
}