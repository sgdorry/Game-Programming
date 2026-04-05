#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include <string>

class Map;

enum EntityType { PLAYER, ENEMY, PLATFORM };
enum AIType { NONE_AI, WANDERER, FOLLOWER, JUMPER, FLYER, DIAGONALER };
enum CollisionDir { COLLIDE_NONE = 0, COLLIDE_TOP = 1, COLLIDE_BOTTOM = 2, COLLIDE_LEFT = 4, COLLIDE_RIGHT = 8 };

struct Entity {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    Vector2 scale;
    Color color;

    EntityType type;
    AIType aiType;

    bool active;
    int collisionFlags;

    float speed;
    float jumpPower;
    float wanderTimer;
    float jumpTimer;
    float wanderDir;
    float verticalDir;
    float animationTimer;
    int animationFrame;
    bool facingRight;
    Entity();
    void update(float dt, Entity* player, Map* map, Entity* platforms, int platformCount);
    void render() const;
    void jump();
    bool isOnGround() const;
    bool checkCollisionWith(const Entity& other) const;
    void resolveCollisions(Entity* platforms, int count);
    void resolveMapCollisionsY(Map* map);
    void resolveMapCollisionsX(Map* map);
    void doAI(float dt, Entity* player, Map* map, Entity* platforms, int platformCount);
};

#endif
