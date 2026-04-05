#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"

enum SceneID {
    SCENE_MENU = 0,
    SCENE_LEVEL1,
    SCENE_LEVEL2,
    SCENE_LEVEL3,
    SCENE_WIN,
    SCENE_LOSE
};

class Scene {
public:
    int nextScene;
    Scene() : nextScene(-1) {}
    virtual ~Scene() {}
    virtual void initialise(int lives) = 0;
    virtual void processInput() = 0;
    virtual void update(float dt) = 0;
    virtual void render(Camera2D* camera) = 0;
    virtual void shutdown() = 0;
};

#endif
