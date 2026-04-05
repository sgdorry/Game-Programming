/**
* Author: Steven Dorry
* Assignment: Rise of the AI
* Date due: 2026-04-04, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "EndScene.h"

EndScene::EndScene(bool w) : won(w), msg(w ? "YOU WIN!": "YOU LOSE") {}
void EndScene::initialise(int lives) { nextScene = -1; }
void EndScene::shutdown() {}
void EndScene::update(float dt) {}
void EndScene::processInput() {
    if (IsKeyPressed(KEY_ENTER)) nextScene = SCENE_MENU;
}
void EndScene::render(Camera2D* camera){
    BeginDrawing();
    ClearBackground(won ? Color{10,30,10,255} :Color{40,10,10,255});
    int w = GetScreenWidth();
    int h = GetScreenHeight() ;
    Color c = won ? Color{100,255,100,255}: Color{255,80,80,255};
    DrawText(msg.c_str(), w/2 - MeasureText(msg.c_str(), 70)/2, h/2 - 60, 70, c);
    DrawText("Press ENTER to return to menu", w/2 - MeasureText("Press ENTER to return to menu", 24)/2, h/2 + 40, 24, {200,200,200,255});
    EndDrawing();
}
