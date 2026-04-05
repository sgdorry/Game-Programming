/**
* Author: Steven Dorry
* Assignment: Rise of the AI
* Date due: 2026-04-04, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "MenuScene.h"

void MenuScene::initialise(int lives) { nextScene = -1; }
void MenuScene::shutdown() {}
void MenuScene::update(float dt) {}

void MenuScene::processInput() {
    if (IsKeyPressed(KEY_ENTER)) nextScene = SCENE_LEVEL1;
}
void MenuScene::render(Camera2D* camera) {
    BeginDrawing();
    ClearBackground({20, 20, 40, 255});
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    DrawText("BACHATA ESCAPE", w/2 - MeasureText("BACHATA ESCAPE", 60)/2, h/2 - 80, 60, {255, 200, 50, 255});
    DrawText("Press ENTER to Start", w/2 - MeasureText("Press ENTER to Start", 28)/2, h/2 + 20, 28, {200, 200, 200, 255});
    DrawText("A/D to move   W to jump", w/2 - MeasureText("A/D to move   W to jump", 20)/2, h/2 + 80, 20, {150, 150, 150, 255});
    EndDrawing();
}
