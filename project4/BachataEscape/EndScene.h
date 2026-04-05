#ifndef END_SCENE_H
#define END_SCENE_H

#include "Scene.h"
#include <string>

class EndScene : public Scene {
    bool won;
    std::string msg;
public:
    EndScene(bool won);
    void initialise(int lives) override;
    void processInput() override;
    void update(float dt) override;
    void render(Camera2D* camera) override;
    void shutdown() override;
};

#endif
