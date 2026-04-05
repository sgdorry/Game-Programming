#ifndef LEVEL_SCENE_H
#define LEVEL_SCENE_H

#include "Scene.h"
#include "Entity.h"
#include "Map.h"
#include <vector>

struct LevelConfig{
    int enemyCount;
    int platformCount;
    float levelWidth;
    Color bgColor;
    Color platformColor;
    Color playerColor;
    Color enemyColors[3];
    float enemySpeed;
    AIType enemyTypes[3];
};

class LevelScene : public Scene{
    int levelIndex;
    int* livesPtr;
    LevelConfig cfg;

    Entity player;
    std::vector<Entity> enemies;
    std::vector<unsigned int> mapData;
    Map* levelMap;

    float hitCooldown;
    bool levelComplete;
    bool levelWinSoundPlayed;
    float levelCompleteTimer;

    void buildLevel();
    void checkEnemyCollisions();
    void checkLevelEnd();

public:
    LevelScene(int index, int* lives);
    void initialise(int lives) override;
    void processInput() override;
    void update(float dt) override;
    void render(Camera2D* camera) override;
    void shutdown() override;
};
#endif