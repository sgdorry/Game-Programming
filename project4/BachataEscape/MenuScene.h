#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include "Scene.h"

class MenuScene : public Scene {
public:
    void initialise(int lives) override;
    void processInput() override;
    void update(float dt) override;
    void render(Camera2D* camera)override;
    void shutdown() override;
};

#endif
