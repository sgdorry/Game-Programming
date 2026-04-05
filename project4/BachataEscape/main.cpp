/**
* Author: Steven Dorry
* Assignment: Rise of the AI
* Date due: 2026-04-04, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "raylib.h"
#include "Scene.h"
#include "MenuScene.h"
#include "LevelScene.h"
#include "EndScene.h"
#include "GameAssets.h"

constexpr int SCREEN_WIDTH  = 1000;
constexpr int SCREEN_HEIGHT = 550;
constexpr int FPS = 60;
constexpr float FIXED_TIMESTEP = 1.0f /60.0f;

int gLives = 3;

Scene* gCurrentScene = nullptr;
MenuScene* gMenu      = nullptr;
LevelScene* gLevel1   = nullptr;
LevelScene* gLevel2   = nullptr;
LevelScene* gLevel3   = nullptr;
EndScene* gWin        = nullptr;
EndScene* gLose       = nullptr;

Camera2D gCamera = {0};
float gPreviousTicks = 0.0f;
float gTimeAccumulator = 0.0f;
Texture2D gPlayerIdleTexture = {0};
Texture2D gPlayerWalkTexture = {0 };
Music gBGM;
Sound gJumpSound;
Sound gHitSound;
Sound gWinSound;
Sound gLoseSound;
bool audioLoaded = false;

void switchToScene(Scene* scene, bool resetLives = false) {
    if (gCurrentScene) gCurrentScene->shutdown();
    if (resetLives) gLives = 3;
    gCurrentScene = scene;
    gCurrentScene->initialise(gLives);
    gCamera.target   = { (float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT / 2.0f };
    gCamera.offset   = { (float)SCREEN_WIDTH / 2.0f,(float)SCREEN_HEIGHT / 2.0f };
    gCamera.rotation =0.0f;
    gCamera.zoom     = 1.0f;
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bachata Escape");
    SetTargetFPS(FPS);
    InitAudioDevice();
    gPlayerIdleTexture = LoadTexture("assets/idle.png");
    gPlayerWalkTexture =LoadTexture("assets/walk.png");
    gBGM = LoadMusicStream("assets/Romeo.mp3");
    if (gBGM.stream.buffer != nullptr) {
        SetMusicVolume(gBGM, 0.4f);
        PlayMusicStream(gBGM);
        audioLoaded = true;
    }
    gJumpSound = LoadSound("assets/YEET.mp3");
    gHitSound  = LoadSound("assets/BRUH.mp3");
    gLoseSound = LoadSound("assets/BRUH.mp3");
    gWinSound  = LoadSound("assets/KOBE.mp3");
    gMenu   = new MenuScene();
    gLevel1 = new LevelScene(0, &gLives);
    gLevel2 = new LevelScene(1, &gLives);
    gLevel3 = new LevelScene(2, &gLives);
    gWin    = new EndScene(true);
    gLose   = new EndScene(false);
    switchToScene(gMenu, true);
    
    while (!WindowShouldClose()) {
        if (audioLoaded) UpdateMusicStream(gBGM);

        float ticks = (float)GetTime();
        float dt = ticks-gPreviousTicks;
        gPreviousTicks =ticks;

        gCurrentScene->processInput();

        dt += gTimeAccumulator;
        if (dt < FIXED_TIMESTEP){
            gTimeAccumulator = dt;
        } 
        else {
            gTimeAccumulator = 0;
            while (dt >= FIXED_TIMESTEP) {
                gCurrentScene->update( FIXED_TIMESTEP);
                dt -= FIXED_TIMESTEP;
            }
        }

        gCurrentScene->render(&gCamera);

        if (gCurrentScene->nextScene >= 0) {
            int next = gCurrentScene->nextScene;
            gCurrentScene->nextScene = -1;

            if (next == SCENE_MENU) {
                switchToScene(gMenu, true);
            } 
            
            else if (next == SCENE_LEVEL1) {
                switchToScene(gLevel1);
            }
            
        else if (next == SCENE_LEVEL2) {
                switchToScene(gLevel2);
            } 
            else if (next == SCENE_LEVEL3) {
                switchToScene(gLevel3);
            } 
            else if (next == SCENE_WIN) {
                switchToScene(gWin);
            } 
            else if (next == SCENE_LOSE) {
                PlaySound(gLoseSound);
                switchToScene(gLose);
            }
    }
    }
    if (gCurrentScene) {
        gCurrentScene->shutdown();
    }
    delete gMenu;
    delete gLevel1;
    delete gLevel2;
    delete gLevel3;
    delete gWin;
    delete gLose;
    if (audioLoaded){
        UnloadMusicStream(gBGM);
    }
    if (gPlayerIdleTexture.id > 0) UnloadTexture(gPlayerIdleTexture);
    if (gPlayerWalkTexture.id> 0) UnloadTexture(gPlayerWalkTexture);
    UnloadSound(gJumpSound);
    UnloadSound(gHitSound);
    UnloadSound(gWinSound);
    UnloadSound(gLoseSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
