#include "MenuScene.h"

MenuScene::MenuScene() {mGameState.nextSceneID = -1;}
void MenuScene::initialise() {mGameState.nextSceneID = -1;}

void MenuScene::update(float deltaTime){
    if (IsKeyPressed(KEY_ENTER)){
        mGameState.nextSceneID = 1;

    }
}

void MenuScene::render(){
    DrawText("BLACKOUT DELIVERY", 320, 180, 56, WHITE);
    DrawText("Press ENTER to start", 470, 280, 24, GRAY);
    DrawText("WASD to move", 520, 380, 22, GRAY);
    DrawText("Collect the pizza, and deliver it before the timer runs out", 320, 420, 20, GRAY);
    DrawText("Avoid pedestrians, stray dogs, and rival cyclists.", 330, 450, 20, GRAY);
}

void MenuScene::shutdown(){}