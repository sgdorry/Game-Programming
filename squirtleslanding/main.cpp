/**
* Author: Steven Dorry
* Assignment: Lunar Lander
* Date due: 03/14/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/Entity.h"
#include <cmath>

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

constexpr float GENGAR_LEFT_BOUND  = 0.0f,
                GENGAR_RIGHT_BOUND = 812.0f,
                GENGAR_Y           = 480.0f;

Entity   *gPlayer;
Entity   *gPokeball;
Entity   *gGengar;
Entity   *gSpikes;

Texture2D gBackgroundTexture;

Vector2  gPlayerVelocity;
float    gPlayerAngle;
float    gPlayerTargetAngle;
float    gFuel;
float    gGengarVelocityX;

bool  gGameOver   = false,
      gMissionWon = false;

float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

void initialise();
void processInput();
void update(float deltaTime);
void render();
void shutdown();
void resetGame();

Vector2 getTextureDimensions(const char *path) {
    Texture2D t = LoadTexture(path);
    Vector2 dims = { (float)t.width, (float)t.height };
    UnloadTexture(t);
    return dims;
}

void resetGame() {
    gPlayer->setPosition({(float)SCREEN_WIDTH / 2.0f, 80.0f});
    gPlayer->setAngle(0.0f);
    gPlayer->activate();
    gPlayerVelocity    = {0.0f, 0.0f};
    gPlayerAngle       = 0.0f;
    gPlayerTargetAngle = 0.0f;
    gFuel              = FUEL_MAX;

    gPokeball->setPosition({701.0f, 954.0f});

    gGengar->setPosition({GENGAR_LEFT_BOUND + 60.0f, GENGAR_Y});
    gGengarVelocityX = GENGAR_SPEED;

    gSpikes[0].setPosition({501.0f, 954.0f});
    gSpikes[1].setPosition({200.0f, 775.0f});

    gGameOver   = false;
    gMissionWon = false;
}

void initialise() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Squirtle's Landing");
    SetTargetFPS(FPS);

    gBackgroundTexture = LoadTexture("background.png");

    Vector2 rocketDims = getTextureDimensions("rocket.png");
    Vector2 gengarDims = getTextureDimensions("gengar.png");

    gPlayer   = new Entity({0.0f, 0.0f}, {rocketDims.x /2.0f, rocketDims.y / 2.0f}, "rocket.png",   PLAYER);
    gPokeball = new Entity({0.0f, 0.0f}, {60.0f, 60.0f},                              "pokeball.png", PLATFORM);
    gGengar   = new Entity({0.0f, 0.0f}, {gengarDims.x,gengarDims.y},                "gengar.png",   BLOCK);

    gSpikes = new Entity[2];

    gSpikes[0].setScale({55.0f, 55.0f});
    gSpikes[0].setColliderDimensions({55.0f, 55.0f});
    gSpikes[0].setTexture("spikes.png");
    gSpikes[0].setEntityType(BLOCK);

    gSpikes[1].setScale({55.0f, 55.0f});
    gSpikes[1].setColliderDimensions({55.0f, 55.0f});
    gSpikes[1].setTexture("spikes.png");
    gSpikes[1].setEntityType(BLOCK);

    resetGame();
}

void processInput() {
    if (gGameOver) {
        if (IsKeyPressed(KEY_R)) resetGame();
        return;
    }

    Vector2 accel = {0.0f, GRAVITY};
    bool hasFuel = gFuel > 0.0f;

    if (IsKeyDown(KEY_SPACE) && hasFuel) {
        accel.y -= THRUST_FORCE;
        gFuel -= FUEL_DRAIN * FIXED_TIMESTEP;
    }

    if (IsKeyDown(KEY_LEFT) && hasFuel) {
        accel.x -= HORIZONTAL_FORCE;
        gFuel -= FUEL_DRAIN * FIXED_TIMESTEP;
        gPlayerTargetAngle = -MAX_TILT;
    } else if (IsKeyDown(KEY_RIGHT) && hasFuel) {
        accel.x += HORIZONTAL_FORCE;
        gFuel -= FUEL_DRAIN * FIXED_TIMESTEP;
        gPlayerTargetAngle = MAX_TILT;
    } else {
        gPlayerTargetAngle = 0.0f;
    }

    if (gFuel < 0.0f) gFuel = 0.0f;

    gPlayer->setAcceleration(accel);
}

void update(float deltaTime) {
    if (gGameOver) return;

    Vector2 playerPos   = gPlayer->getPosition();
    Vector2 playerAccel = gPlayer->getAcceleration();
    Vector2 playerScale = gPlayer->getScale();

    gPlayerVelocity.x += playerAccel.x * deltaTime;
    gPlayerVelocity.y += playerAccel.y * deltaTime;
    gPlayerVelocity.x -= gPlayerVelocity.x * DRAG * deltaTime;

    playerPos.x += gPlayerVelocity.x * deltaTime;
    playerPos.y += gPlayerVelocity.y * deltaTime;

    float angleDiff = gPlayerTargetAngle - gPlayerAngle;
    gPlayerAngle += angleDiff * TILT_SPEED * deltaTime * 0.05f;
    gPlayer->setAngle(gPlayerAngle);

    gPlayer->setPosition(playerPos);

    Vector2 gengarPos = gGengar->getPosition();
    gengarPos.x += gGengarVelocityX * deltaTime;
    if (gengarPos.x >= GENGAR_RIGHT_BOUND || gengarPos.x <= GENGAR_LEFT_BOUND) {
        gGengarVelocityX *= -1.0f;
    }
    gGengar->setPosition(gengarPos);

    float halfW = playerScale.x / 2.0f;
    float halfH = playerScale.y / 2.0f;
    if (playerPos.x - halfW < 0.0f   ||
        playerPos.x + halfW > (float)SCREEN_WIDTH  ||
        playerPos.y - halfH < 0.0f   ||
        playerPos.y + halfH > (float)SCREEN_HEIGHT) {
        gGameOver   = true;
        gMissionWon = false;
        return;
    }

    if (gPlayer->isColliding(gPokeball)) {
        gGameOver   = true;
        gMissionWon = true;
        return;
    }

    if (gPlayer->isColliding(gGengar)) {
        gGameOver   = true;
        gMissionWon = false;
        return;
    }

    for (int i = 0; i < 2; i++) {
        if (gPlayer->isColliding(&gSpikes[i])) {
            gGameOver   = true;
            gMissionWon = false;
            return;
        }
    }
}

void render() {
    BeginDrawing();
    ClearBackground(BLACK);

    Rectangle bgSrc = {0.0f, 0.0f, (float)gBackgroundTexture.width, (float)gBackgroundTexture.height};
    Rectangle bgDst = {0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
    DrawTexturePro(gBackgroundTexture, bgSrc, bgDst, {0.0f, 0.0f}, 0.0f, WHITE);

    gPokeball->render();
    gGengar->render();
    gSpikes[0].render();
    gSpikes[1].render();
    gPlayer->render();

    float fuelRatio = gFuel/ FUEL_MAX;
    Color barColor  = (fuelRatio > 0.55f) ? GREEN : (fuelRatio > 0.25f) ? YELLOW : RED;

    char fuelText[32];
    snprintf(fuelText, sizeof(fuelText), "FUEL: %d%%", (int)(fuelRatio * 100.0f));
    DrawText("FUEL", 15, 1, 14, WHITE);
    DrawText(fuelText, 15, 15, 20, barColor);

    if (gGameOver) {
        const char *msg = gMissionWon ? "MISSION ACCOMPLISHED" : "MISSION FAILED!";
        Color       msgColor = gMissionWon ? GREEN : RED;
        DrawText(msg, 200, SCREEN_HEIGHT / 2 - 24, 32, msgColor);
        DrawText("Press R to restart", 285, SCREEN_HEIGHT / 2 + 20, 20, WHITE);
    }

    EndDrawing();
}

void shutdown() {
    UnloadTexture(gBackgroundTexture);

    delete gPlayer;
    delete gPokeball;
    delete gGengar;
    delete[] gSpikes;
    CloseWindow();
}

int main() {
    initialise();

    while (!WindowShouldClose()) {
        float ticks     = (float)GetTime();
        float deltaTime = ticks - gPreviousTicks;
        gPreviousTicks  = ticks;

        deltaTime += gTimeAccumulator;
        if (deltaTime < FIXED_TIMESTEP) {
            gTimeAccumulator = deltaTime;
        } else {
            while (deltaTime >=FIXED_TIMESTEP) {
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
