/**
* Author: Steven Dorry
* Assignment: Lunar Lander
* Date due: 03/14/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "/opt/homebrew/opt/raylib/include/raylib.h"
#include <cmath>

// Global Constants
constexpr int   SCREEN_WIDTH   = 812,
                SCREEN_HEIGHT  = 1080,
                FPS            = 60;

constexpr float GRAVITY            = 13.0f,
                THRUST_FORCE       = 50.0f,
                HORIZONTAL_FORCE   = 300.0f,
                DRAG               = 1.5f, 
                MAX_TILT           = 25.0f, 
                TILT_SPEED         = 90.0f,
                FUEL_MAX           = 100.0f,
                FUEL_DRAIN         = 7.0f,
                GENGAR_SPEED       = 340.0f,
                FIXED_TIMESTEP     = 1.0f / 60.0f;

constexpr float GENGAR_LEFT_BOUND   = 0.0f,
                GENGAR_RIGHT_BOUND  = 812.0f,
                GENGAR_Y            = 480.0f;

struct Player {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    Vector2 size;
    float   angle;          
    float   targetAngle;    
    float   fuel;
    bool    isActive;
};

struct Platform {
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    bool    isMoving;
    bool    isWinPlatform;
};


Player    gPlayer;
Platform  gPokeball;
Platform  gGengar;
Platform  gSpikes[2];

Texture2D gRocketTexture,
          gPokeballTexture,
          gGengarTexture,
          gSpikesTexture,
          gBackgroundTexture;

bool  gGameOver    = false,
      gMissionWon  = false;

float gPreviousTicks  = 0.0f,
      gTimeAccumulator = 0.0f;


void initialise();
void processInput();
void update(float deltaTime);
void render();
void shutdown();
void resetGame();

bool isColliding(const Vector2 *positionA, const Vector2 *scaleA,
                 const Vector2 *positionB, const Vector2 *scaleB);
void renderObject(const Texture2D *texture, const Vector2 *position,
                  const Vector2 *size, float angle);


bool isColliding(const Vector2 *positionA, const Vector2 *scaleA,
                 const Vector2 *positionB, const Vector2 *scaleB){
    float xDist = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDist = fabs(positionA->y -positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);
    return (xDist < 0.0f && yDist < 0.0f);
}


void renderObject(const Texture2D *texture, const Vector2 *position,
                  const Vector2 *size, float angle){
    Rectangle src = {0.0f, 0.0f, (float)texture->width, (float)texture->height};
    Rectangle dst = {position->x, position->y, size->x,size->y};
    Vector2 origin = {size->x / 2.0f, size->y / 2.0f};
    DrawTexturePro(*texture, src, dst, origin, angle, WHITE);
}


void resetGame()
{
    // Rocket squirtle
    gPlayer.position = {(float)SCREEN_WIDTH / 2.0f, 80.0f};
    gPlayer.velocity = {0.0f, 0.0f};
    gPlayer.acceleration = {0.0f, 0.0f};
    gPlayer.size = {(float)gRocketTexture.width/2, (float)gRocketTexture.height/2};
    gPlayer.angle = 0.0f;
    gPlayer.targetAngle = 0.0f;
    gPlayer.fuel = FUEL_MAX;
    gPlayer.isActive = true;

    // Pokeball
    gPokeball.position = {701.0f, 954.0f};
    gPokeball.size = {60.0f, 60.0f };
    gPokeball.velocity = {0.0f, 0.0f};
    gPokeball.isMoving = false;
    gPokeball.isWinPlatform = true;

    // Gengar
    gGengar.position = {GENGAR_LEFT_BOUND + 60.0f, GENGAR_Y};
    gGengar.size = {(float)gGengarTexture.width,(float)gGengarTexture.height};
    gGengar.velocity = {GENGAR_SPEED, 0.0f};
    gGengar.isMoving = true;
    gGengar.isWinPlatform = false;

    // Spike1
    gSpikes[0].position = {501.0f, 954.0f};
    gSpikes[0].size = {55.0f, 55.0f};
    gSpikes[0].velocity = {0.0f, 0.0f};
    gSpikes[0].isMoving = false;
    gSpikes[0].isWinPlatform = false;


    //Spike2
    gSpikes[1].position = {200.0f, 775.0f};
    gSpikes[1].size = {55.0f, 55.0f};
    gSpikes[1].velocity = {0.0f, 0.0f};
    gSpikes[1].isMoving = false;
    gSpikes[1].isWinPlatform = false;

    gGameOver = false;
    gMissionWon = false;
}


void initialise(){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Squirtle's Landing");
    SetTargetFPS(FPS);
    gBackgroundTexture = LoadTexture("background.png");
    gRocketTexture = LoadTexture("rocket.png");
    gPokeballTexture = LoadTexture("pokeball.png");
    gGengarTexture = LoadTexture("gengar.png");
    gSpikesTexture = LoadTexture("spikes.png");
    resetGame();
}

void processInput(){
    if (gGameOver) {
        if (IsKeyPressed(KEY_R)){
            resetGame();
        }
        return;
    }

    gPlayer.acceleration = {0.0f, GRAVITY};

    bool hasFuel = gPlayer.fuel > 0.0f;

    if (IsKeyDown(KEY_SPACE) && hasFuel) {
        gPlayer.acceleration.y -= THRUST_FORCE;
        gPlayer.fuel -= FUEL_DRAIN*FIXED_TIMESTEP;
    }

    if (IsKeyDown(KEY_LEFT) && hasFuel) {
        gPlayer.acceleration.x -= HORIZONTAL_FORCE;
        gPlayer.fuel -= FUEL_DRAIN * FIXED_TIMESTEP;
        gPlayer.targetAngle = -MAX_TILT;
    }
    else if (IsKeyDown(KEY_RIGHT) && hasFuel) {
        gPlayer.acceleration.x += HORIZONTAL_FORCE;
        gPlayer.fuel -= FUEL_DRAIN * FIXED_TIMESTEP;
        gPlayer.targetAngle =MAX_TILT;
    }
    else {
        gPlayer.targetAngle = 0.0f;
    }

    if (gPlayer.fuel < 0.0f){
        gPlayer.fuel = 0.0f;
    }
}


void update(float deltaTime){
    if (gGameOver || !gPlayer.isActive){
        return;
    }

    gPlayer.velocity.x += gPlayer.acceleration.x * deltaTime;
    gPlayer.velocity.y += gPlayer.acceleration.y * deltaTime;

    gPlayer.velocity.x -= gPlayer.velocity.x * DRAG * deltaTime;

    gPlayer.position.x += gPlayer.velocity.x * deltaTime;
    gPlayer.position.y += gPlayer.velocity.y * deltaTime;

    float angleDiff = gPlayer.targetAngle - gPlayer.angle;
    gPlayer.angle += angleDiff * TILT_SPEED * deltaTime * 0.05f;

    // Gengar movement
    gGengar.position.x += gGengar.velocity.x * deltaTime;
    if (gGengar.position.x >= GENGAR_RIGHT_BOUND || gGengar.position.x <= GENGAR_LEFT_BOUND) {
        gGengar.velocity.x *= -1.0f;
    }

    float halfW = gPlayer.size.x / 2.0f;
    float halfH = gPlayer.size.y / 2.0f;
    if (gPlayer.position.x - halfW < 0.0f   ||
        gPlayer.position.x + halfW > (float)SCREEN_WIDTH ||
        gPlayer.position.y - halfH < 0.0f   ||
        gPlayer.position.y + halfH > (float)SCREEN_HEIGHT){
        gGameOver = true;
        gMissionWon= false;
        return;
    }

    if (isColliding(&gPlayer.position, &gPlayer.size,
                    &gPokeball.position, &gPokeball.size)){
        gGameOver = true;
        gMissionWon = true;
        return;
    }

    if (isColliding(&gPlayer.position, &gPlayer.size,
                    &gGengar.position, &gGengar.size)){
        gGameOver = true;
        gMissionWon = false;
        return;
    }

    for (int i = 0; i <2; i++){
        if (isColliding(&gPlayer.position, &gPlayer.size, &gSpikes[i].position, &gSpikes[i].size)){
            gGameOver = true;
            gMissionWon = false;
            return;
        }
    }
}


void render(){
    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexture(gBackgroundTexture, 0, 0, WHITE);

    renderObject(&gPokeballTexture, &gPokeball.position, &gPokeball.size, 0.0f);
    renderObject(&gGengarTexture, &gGengar.position, &gGengar.size, 0.0f);
    renderObject(&gSpikesTexture, &gSpikes[0].position, &gSpikes[0].size, 0.0f);
    renderObject(&gSpikesTexture, &gSpikes[1].position, &gSpikes[1].size, 0.0f);


    renderObject(&gRocketTexture, &gPlayer.position, &gPlayer.size, gPlayer.angle);

    constexpr float BAR_X      = 15.0f,
                    BAR_Y      = 15.0f,
                    BAR_W      = 140.0f,
                    BAR_H      = 16.0f;

    float fuelRatio = gPlayer.fuel / FUEL_MAX;

    Color barColor;
    
    if (fuelRatio > 0.55f){
        barColor = GREEN;
    }   
    else if (fuelRatio > 0.25f){
        barColor = YELLOW;
    }
    else{
        barColor = RED;
    }

    DrawText("FUEL", (int)BAR_X, (int)(BAR_Y - 14), 14, WHITE);
    DrawRectangle((int)BAR_X, (int)BAR_Y, (int)BAR_W, (int)BAR_H, DARKGRAY);
    DrawRectangle((int)BAR_X, (int)BAR_Y, (int)(BAR_W * fuelRatio), (int)BAR_H, barColor);
    DrawRectangleLines((int)BAR_X, (int)BAR_Y, (int)BAR_W, (int)BAR_H, WHITE);

    // End screen
    if (gGameOver) {
        const char *msg = gMissionWon ? "MISSION ACCOMPLISHED" : "MISSION FAILED!";
        Color msgColor = gMissionWon ? GREEN : RED;
        int   fontSize = 32;
        int   msgW = MeasureText(msg, fontSize);
        DrawText(msg, SCREEN_WIDTH / 2 - msgW / 2, SCREEN_HEIGHT / 2 - 24, fontSize, msgColor);
        const char *restart = "Press R to restart";
        int restartW = MeasureText(restart, 20);
        DrawText(restart, SCREEN_WIDTH / 2 - restartW / 2, SCREEN_HEIGHT /2 + 20, 20, WHITE);
    }
    EndDrawing();
}


void shutdown(){
    UnloadTexture(gBackgroundTexture);
    UnloadTexture(gRocketTexture);
    UnloadTexture(gPokeballTexture);
    UnloadTexture(gGengarTexture);
    UnloadTexture(gSpikesTexture);

    CloseWindow();
}

int main(){
    initialise();

    while (!WindowShouldClose()) {
        float ticks = (float)GetTime();
        float deltaTime = ticks - gPreviousTicks;
        gPreviousTicks = ticks;

        deltaTime+= gTimeAccumulator;
        if (deltaTime < FIXED_TIMESTEP) {
            gTimeAccumulator = deltaTime;
        } else{
            while (deltaTime >= FIXED_TIMESTEP) {
                processInput();
                update(FIXED_TIMESTEP);
                deltaTime -= FIXED_TIMESTEP;
            }
            gTimeAccumulator = deltaTime;
        }
        render();
    }
    shutdown();
    return 0;
}