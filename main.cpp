/**
* Author: Steven Dorry
* Assignment: Simple 2D Scene
* Date due: 02/14/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "/opt/homebrew/opt/raylib/include/raylib.h"
#include <math.h>

// Enums
enum AppStatus { TERMINATED, RUNNING };

// Global Constants
constexpr int SCREEN_WIDTH  = 800,
              SCREEN_HEIGHT = 450,
              FPS           = 60;

// Global Variables
AppStatus gAppStatus   = RUNNING;

float ballX = 400.0f;
float ballY = 225.0f;

float gBallRotation = 0.0f;
float gStarScale = 1.0f;
float gStarPulseTime = 0.0f;

float gStarAngle = 0.0f;
float gStarRadius = 100.0f;
float gPreviousTicks = 0.0f;


Texture2D gGrassTexture;
Texture2D gBallTexture;
Texture2D gLinesTexture;
Texture2D gStarTexture;



// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Soccer!");
    gGrassTexture = LoadTexture("/Users/dorry/Desktop/Game Programming/soccer/grassbackground.png");
    gBallTexture = LoadTexture("/Users/dorry/Desktop/Game Programming/soccer/Soccerball.png");
    gLinesTexture = LoadTexture("/Users/dorry/Desktop/Game Programming/soccer/pitchlines.png");
    gStarTexture = LoadTexture("/Users/dorry/Desktop/Game Programming/soccer/star.png");
    SetTargetFPS(FPS);
}

void processInput(){
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() {

    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    float ballOrbitAngle = ticks * 30.0f;
    float ballOrbitRadius = 75.0f;

    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;


    ballX = centerX + ballOrbitRadius * cos(ballOrbitAngle * 3.14159265358979 / 180.0f);
    ballY = centerY + ballOrbitRadius * sin(ballOrbitAngle * 3.14159265358979 / 180.0f);


    gBallRotation += 90.0f * deltaTime;
    gStarAngle += 120.0f * deltaTime;

    gStarPulseTime += 1.0f * deltaTime;
    gStarScale = 1.0f + 0.2f * cos(gStarPulseTime);

}

void render()
{
    BeginDrawing();
    DrawTexturePro(
        gGrassTexture,
        Rectangle{0, 0, (float)gGrassTexture.width, (float)gGrassTexture.height},
        Rectangle{0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
        Vector2{0,0}, 0.0f, WHITE
    );
    // pitch lines

    DrawTexturePro(
        gLinesTexture,
        Rectangle{0, 0, (float)gLinesTexture.width, (float) gLinesTexture.height},
        Rectangle{0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
        Vector2{0,0}, 0.0f, WHITE
    );

    // star
    
    float starX = ballX + gStarRadius * cos(gStarAngle * 3.14159265358979f / 180.0f);
    float starY = ballY + gStarRadius * sin(gStarAngle * 3.14159265358979f / 180.0f);
    float starSize = 50.0f * gStarScale;

    DrawTexturePro(
        gStarTexture,
        Rectangle{0, 0, (float)gStarTexture.width, (float) gStarTexture.height},
        Rectangle{starX - starSize/2, starY - starSize/2, starSize, starSize},
        Vector2{0,0}, 0.0f, WHITE
    );
    // ball
    float ballSize = 50.0f;
    DrawTexturePro(
        gBallTexture,
        Rectangle{0,0, (float)gBallTexture.width, (float)gBallTexture.height},
        Rectangle{ballX-ballSize/2, ballY-ballSize/2, ballSize, ballSize},
        Vector2{ballSize/2, ballSize/2}, gBallRotation, WHITE
    );
    EndDrawing();

}

void shutdown() 
{ 
    UnloadTexture(gGrassTexture);
    UnloadTexture(gBallTexture);
    UnloadTexture(gLinesTexture);
    UnloadTexture(gStarTexture);
    CloseWindow(); // Close window and OpenGL context
}

int main(void){
    initialise();


    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }


    shutdown();

    return 0;
}
