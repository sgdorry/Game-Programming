/**
* Author: Steven Dorry
* Assignment: Rise of the AI
* Date due: 2026-04-04, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelScene.h"
#include "GameAssets.h"
#include <cmath>

LevelScene::LevelScene(int index, int* lives)
    : levelIndex(index), livesPtr(lives), levelMap(nullptr), hitCooldown(0), levelComplete(false), levelWinSoundPlayed(false), levelCompleteTimer(0) {}

void LevelScene::initialise(int lives) {
    nextScene = -1;
    hitCooldown = 0;
    levelComplete = false;
    levelWinSoundPlayed = false;
    levelCompleteTimer = 0;
    if (levelMap) {
        delete levelMap;
        levelMap = nullptr;
    }
    enemies.clear();
    mapData.clear();

    if (levelIndex == 0) {
        cfg.bgColor = {30, 30, 60, 255};
        cfg.platformColor = {80, 80, 160, 255};
        cfg.playerColor = {100, 220, 100, 255};
        cfg.levelWidth = 3000.0f;
        cfg.enemyCount = 2;
        cfg.enemySpeed = 100.0f;
        cfg.enemyTypes[0] = WANDERER;
        cfg.enemyColors[0] = {220, 80, 80, 255};
    } else if (levelIndex == 1) {
        cfg.bgColor = {20, 50, 30, 255};
        cfg.platformColor = {60, 140, 80, 255};
        cfg.playerColor = {100, 220, 100, 255};
        cfg.levelWidth = 4000.0f;
        cfg.enemyCount = 4;
        cfg.enemySpeed = 350.0f;
        cfg.enemyTypes[0] = FLYER;
        cfg.enemyTypes[1] = FLYER;
        cfg.enemyColors[0] = {220, 80, 80, 255};
        cfg.enemyColors[1] = {220, 160, 40, 255};
    } else {
        cfg.bgColor = {50, 20, 20, 255};
        cfg.platformColor = {160, 60, 60, 255};
        cfg.playerColor = {100, 220, 100, 255};
        cfg.levelWidth = 5000.0f;
        cfg.enemyCount = 6;
        cfg.enemySpeed = 400.0f;
        cfg.enemyTypes[0] = DIAGONALER;
        cfg.enemyTypes[1] = DIAGONALER;
        cfg.enemyTypes[2] = DIAGONALER;
        cfg.enemyColors[0] = {220, 80, 80, 255};
        cfg.enemyColors[1] = {220, 160, 40, 255};
        cfg.enemyColors[2] = {180, 60, 220, 255};
    }

    buildLevel();
}

void LevelScene::buildLevel() {
    int sh = GetScreenHeight();

    player.position = {200.0f, (float)sh - 200.0f};
    player.velocity = {0, 0};
    player.scale = {40.0f, 55.0f};
    player.color = cfg.playerColor;
    player.type = PLAYER;
    player.aiType = NONE_AI;
    player.acceleration = {0, 981.0f};
    player.speed = 220.0f;
    player.jumpPower = 520.0f;
    player.collisionFlags = COLLIDE_NONE;

    float ground = (float)sh - 40.0f;
    float lw = cfg.levelWidth;

    const float mapTileSize = 20.0f;
    int mapColumns = (int)((lw + 400.0f) / mapTileSize);
    if (((float)mapColumns * mapTileSize) < (lw + 400.0f)) mapColumns++;
    int mapRows = 32;
    mapData.assign(mapColumns * mapRows, 0);

    auto makePlatform = [&](float x, float y, float w, float h) {
        int startCol = (int)floorf(x / mapTileSize);
        int endCol = (int)ceilf((x + w) / mapTileSize) - 1;
        int startRow = (int)floorf(y / mapTileSize);
        int endRow = (int)ceilf((y + h) / mapTileSize) - 1;

        if (startCol < 0) startCol = 0;
        if (startRow < 0) startRow = 0;
        if (endCol >= mapColumns) endCol = mapColumns - 1;
        if (endRow >= mapRows) endRow = mapRows - 1;

        for (int row = startRow; row <= endRow; row++) {
            for (int col = startCol; col <= endCol; col++) {
                mapData[row * mapColumns + col] = 1;
            }
        }
    };

    levelMap = new Map(
        mapColumns,
        mapRows,
        mapData.data(),
        "",
        mapTileSize,
        1,
        1,
        Vector2{((float)mapColumns * mapTileSize) / 2.0f, ((float)mapRows * mapTileSize) / 2.0f}
    );
    levelMap->setFallbackColor(cfg.platformColor);

    makePlatform(0, ground, lw + 400.0f, 80.0f);

    if (levelIndex == 0) {
        makePlatform(300, ground - 130, 200, 20);
        makePlatform(600, ground - 200, 200, 20);
        makePlatform(900, ground - 130, 200, 20);
        makePlatform(1200, ground - 260, 200, 20);
        makePlatform(1500, ground - 180, 200, 20);
        makePlatform(1800, ground - 130, 200, 20);
        makePlatform(2100, ground - 220, 200, 20);
        makePlatform(2400, ground - 150, 200, 20);

        float levelOneXs[] = {700.0f, 1900.0f};
        for (int i = 0; i < 2; i++) {
            Entity e;
            e.position = {levelOneXs[i], ground - 70.0f};
            e.velocity = {0, 0};
            e.scale = {40.0f, 55.0f};
            e.color = (i == 0) ? cfg.enemyColors[0] : Color{220, 140, 90, 255};
            e.type = ENEMY;
            e.aiType = WANDERER;
            e.acceleration = {0, 981.0f};
            e.speed = cfg.enemySpeed + i * 15.0f;
            e.jumpPower = 400.0f;
            e.wanderDir = (i % 2 == 0) ? 1.0f : -1.0f;
            enemies.push_back(e);
        }
    } 
    else if (levelIndex == 1) {
        makePlatform(300, ground - 150, 180, 20);
        makePlatform(600, ground - 230, 180, 20);
        makePlatform(850, ground - 150, 150, 20);
        makePlatform(1100, ground - 280, 200, 20);
        makePlatform(1400, ground - 200, 180, 20);
        makePlatform(1700, ground - 150, 160, 20);
        makePlatform(2000, ground - 300, 200, 20);
        makePlatform(2300, ground - 180, 180, 20);
        makePlatform(2600, ground - 240, 180, 20);
        makePlatform(2900, ground -160, 200, 20);
        makePlatform(3200, ground -290, 200, 20);

        Color flyerColors[] = {
            Color{220, 80, 80, 255},
            Color{220, 160, 40, 255},
            Color{120, 220, 220, 255},
            Color{240, 120, 200, 255}
        };
        Vector2 flyerPositions[] = {
            {700.0f, ground - 320.0f},
            {1500.0f, ground - 360.0f},
            {2400.0f, ground - 300.0f},
            {3200.0f, ground - 380.0f}
        };
        for (int i = 0; i < 4; i++) {
            Entity e;
            e.position = flyerPositions[i];
            e.velocity = {0, 0};
            e.scale = {38.0f, 38.0f};
            e.color = flyerColors[i];
            e.type = ENEMY;
            e.aiType = FLYER;
            e.acceleration = {0, 0};
            e.speed = cfg.enemySpeed + i * 8.0f;
            e.jumpPower = 0.0f;
            e.verticalDir = (i % 2 == 0) ? 1.0f : -1.0f;
            enemies.push_back(e);
        }
    } else {
        makePlatform(300, ground - 160, 160, 20);
        makePlatform(550, ground - 260, 160, 20);
        makePlatform(800, ground - 180, 130, 20);
        makePlatform(1050, ground - 310, 160, 20);
        makePlatform(1300, ground - 220, 150, 20);
        makePlatform(1580, ground - 170, 140, 20);
        makePlatform(1830, ground - 320, 180, 20);
        makePlatform(2100, ground - 200, 150, 20);
        makePlatform(2370, ground - 280, 160, 20);
        makePlatform(2640, ground - 160, 150, 20);
        makePlatform(2900, ground - 340, 180, 20);
        makePlatform(3180, ground - 220, 160, 20);
        makePlatform(3450, ground - 180, 150, 20);
        makePlatform(3720, ground - 300, 180, 20);

        Color diagonalerColors[] = {
            Color{220, 80, 80, 255},
            Color{220, 160, 40, 255},
            Color{180, 60, 220, 255},
            Color{80, 180, 240, 255},
            Color{255, 100, 180, 255},
            Color{160, 255, 120, 255}
        };
        Vector2 diagonalerPositions[] = {
            {450.0f, ground - 360.0f},
            {1050.0f, ground - 430.0f},
            {1650.0f, ground - 300.0f},
            {2450.0f, ground - 410.0f},
            {3300.0f, ground - 320.0f},
            {4100.0f, ground - 440.0f}
        };
        for (int i = 0; i < 6; i++) {
            Entity e;
            e.position = diagonalerPositions[i];
            e.velocity = {0, 0};
            e.scale = {36.0f, 36.0f};
            e.color = diagonalerColors[i];
            e.type = ENEMY;
            e.aiType = DIAGONALER;
            e.acceleration = {0, 0};
            e.speed = cfg.enemySpeed + (i % 3) * 10.0f;
            e.jumpPower = 0.0f;
            e.wanderDir = (i % 2 == 0) ? 1.0f : -1.0f;
            e.verticalDir = (i % 3 == 0) ? 1.0f : -1.0f;
            enemies.push_back(e);
        }
    }
}

void LevelScene::processInput() {
    if (IsKeyPressed(KEY_ONE))   { nextScene = SCENE_LEVEL1; return; }
    if (IsKeyPressed(KEY_TWO))   { nextScene = SCENE_LEVEL2; return; }
    if (IsKeyPressed(KEY_THREE)) { nextScene = SCENE_LEVEL3; return; }

    player.velocity.x = 0;
    if (IsKeyDown(KEY_A)) player.velocity.x = -player.speed;
    if (IsKeyDown(KEY_D)) player.velocity.x = player.speed;
    if (IsKeyPressed(KEY_W) && player.isOnGround()) {
        player.jump();
        PlaySound(gJumpSound);
    }
    if (IsKeyPressed(KEY_SPACE) && player.isOnGround()) {
        player.jump();
        PlaySound(gJumpSound);
    }
}

void LevelScene::checkEnemyCollisions() {
    if (hitCooldown > 0) return;
    for (auto& e : enemies) {
        if (!e.active) continue;
        if (player.checkCollisionWith(e)) {
            (*livesPtr)--;
            hitCooldown = 1.5f;
            PlaySound(gHitSound);
            if (*livesPtr <= 0) nextScene = SCENE_LOSE;
            return;
        }
    }
}

void LevelScene::checkLevelEnd() {
    if (player.position.x > cfg.levelWidth - 100.0f) {
        levelComplete = true;
        if (!levelWinSoundPlayed) {
            PlaySound(gWinSound);
            levelWinSoundPlayed = true;
        }
    }
    int sh = GetScreenHeight();
    if (player.position.y > sh + 200.0f) {
        (*livesPtr)--;
        PlaySound(gHitSound);
        if (*livesPtr <= 0) nextScene = SCENE_LOSE;
        else {
            player.position = {200.0f, (float)sh - 200.0f};
            player.velocity = {0, 0};
            hitCooldown = 1.0f;
        }
    }
}

void LevelScene::update(float dt) {
    if (nextScene >= 0) return;

    if (hitCooldown > 0) hitCooldown -= dt;

    player.update(dt, nullptr, levelMap, nullptr, 0);

    for (auto& e : enemies) {
        e.update(dt, &player, levelMap, nullptr, 0);
    }

    checkEnemyCollisions();
    checkLevelEnd();

    if (levelComplete) {
        levelCompleteTimer += dt;
        if (levelCompleteTimer > 1.0f) {
            if (levelIndex == 0) nextScene = SCENE_LEVEL2;
            else if (levelIndex == 1) nextScene = SCENE_LEVEL3;
            else nextScene = SCENE_WIN;
        }
    }
}

void LevelScene::render(Camera2D* camera) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    camera->target = {player.position.x, (float)sh / 2.0f};
    camera->offset = {(float)sw / 2.0f, (float)sh / 2.0f};

    BeginDrawing();
    ClearBackground(cfg.bgColor);
    BeginMode2D(*camera);

    if (levelMap) levelMap->render();
    for (auto& e : enemies) e.render();

    if (hitCooldown <= 0 || (int)(hitCooldown * 10) % 2 == 0)
        player.render();
    float finishX = cfg.levelWidth - 80.0f;
    float ground = (float)sh - 40.0f;
    DrawRectangle((int)finishX, (int)(ground - 120), 20, 120, {255, 255, 50, 255});
    DrawRectangle((int)finishX, (int)(ground - 130), 60, 20, {255, 255, 50, 255});
    EndMode2D();
    DrawText(TextFormat("Lives: %d", *livesPtr), 20, 20, 30, {255, 200, 200, 255});
    DrawText(TextFormat("Level: %d", levelIndex + 1), 20, 55, 24, {200, 200, 255, 255});

    if (levelComplete)
        DrawText("Level Complete!", sw/2 - MeasureText("Level Complete!", 40)/2, sh/2 - 20, 40, {255, 255, 100, 255});

    EndDrawing();
}

void LevelScene::shutdown() {
    if (levelMap) {
        delete levelMap;
        levelMap = nullptr;
    }
    mapData.clear();
    enemies.clear();
}