/**
* Author: Steven Dorry
* Assignment: Rise of the AI
* Date due: 2026-04-04, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "Entity.h"
#include "GameAssets.h"
#include "Map.h"
#include <cmath>

Entity::Entity()
    : position({0,0}), velocity({0,0}), acceleration({0,981.0f}),
      scale({50,50}), color(WHITE), type(PLATFORM), aiType(NONE_AI),
      active(true), collisionFlags(COLLIDE_NONE), speed(200.0f),
      jumpPower(500.0f), wanderTimer(0), jumpTimer(0), wanderDir(1.0f), verticalDir(1.0f),
      animationTimer(0), animationFrame(0), facingRight(true) {}

bool Entity::isOnGround() const{
    return (collisionFlags & COLLIDE_BOTTOM) != 0;
}
bool Entity::checkCollisionWith(const Entity& other) const {
    float ax = position.x - scale.x / 2.0f;
    float ay = position.y - scale.y / 2.0f;
    float bx = other.position.x - other.scale.x / 2.0f;
    float by = other.position.y - other.scale.y / 2.0f;
    return ax < bx + other.scale.x && ax + scale.x > bx &&
           ay < by + other.scale.y && ay + scale.y > by;
}
void Entity::resolveCollisions(Entity* platforms, int count) {
    collisionFlags = COLLIDE_NONE;
    for (int i = 0; i < count; i++) {
        if (!platforms[i].active) continue;
        if (!checkCollisionWith(platforms[i])) continue;
        float px = position.x, py = position.y;
        float sx = scale.x, sy =scale.y;
        float ox = platforms[i].position.x, oy = platforms[i].position.y;
        float osx = platforms[i].scale.x, osy = platforms[i].scale.y;

        float overlapX = (sx / 2.0f + osx / 2.0f) - fabsf(px - ox);
        float overlapY = (sy / 2.0f + osy / 2.0f) - fabsf(py - oy);
        if (overlapX < overlapY) {
            if (px < ox) {
                position.x -= overlapX;
                collisionFlags |= COLLIDE_RIGHT;
            } 
            else {
                position.x += overlapX;
                collisionFlags |= COLLIDE_LEFT;
            }
            velocity.x = 0;
        } else {
            if (py < oy) {
                position.y -= overlapY;
                collisionFlags |= COLLIDE_BOTTOM;
                velocity.y = 0;
            } else {
                position.y += overlapY;
                collisionFlags |= COLLIDE_TOP;
                velocity.y = 0;
            }
      }
}
}

void Entity::jump() {
    if (isOnGround()) velocity.y = -jumpPower;
}

void Entity::resolveMapCollisionsY(Map* map) {
    if (!map) return;

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;
    float maxOverlap = 0.0f;
    bool collided = false;
    const float probeOffsetX = scale.x * 0.3f;
    if (velocity.y > 0.0f) {
        Vector2 probes[2] = {
            { position.x - probeOffsetX, position.y + scale.y / 2.0f },
            { position.x + probeOffsetX, position.y + scale.y / 2.0f }
        };

        for (const Vector2& probe : probes) {
            if (map->isSolidTileAt(probe, &xOverlap, &yOverlap)) {
                collided = true;
                if (yOverlap > maxOverlap) maxOverlap = yOverlap;
            }
        }
        if (collided) {
            position.y -= maxOverlap;
            velocity.y = 0.0f;
            collisionFlags |= COLLIDE_BOTTOM;
        }
    } else if (velocity.y < 0.0f) {
        Vector2 probes[2] = {
            {position.x - probeOffsetX, position.y - scale.y / 2.0f },
            {position.x + probeOffsetX, position.y - scale.y / 2.0f}
    };
        for (const Vector2& probe : probes) {
            if (map->isSolidTileAt(probe, &xOverlap, &yOverlap)) {
                collided = true;
                if (yOverlap > maxOverlap) maxOverlap = yOverlap;
            }
        }
        if (collided) {
            position.y += maxOverlap;
            velocity.y = 0.0f;
            collisionFlags |= COLLIDE_TOP;
    }
}
}

void Entity::resolveMapCollisionsX(Map* map){
    if (!map) return;
    float xOverlap =0.0f;
    float yOverlap = 0.0f;
    float maxOverlap = 0.0f;
    bool collided = false;
    const float probeOffsetY = scale.y * 0.3f;

    if (velocity.x > 0.0f) {
        Vector2 probes[2] = {
            {position.x + scale.x / 2.0f, position.y - probeOffsetY },
            { position.x + scale.x / 2.0f, position.y + probeOffsetY }
        };
     for (const Vector2& probe :probes) {
            if (map->isSolidTileAt(probe, &xOverlap, &yOverlap)) {
                collided = true;
                if (xOverlap > maxOverlap) maxOverlap = xOverlap;
            }
        }
        if (collided) {
            position.x -= maxOverlap;
            velocity.x = 0.0f;
            collisionFlags |= COLLIDE_RIGHT;
        }
    } else if (velocity.x < 0.0f) {
        Vector2 probes[2] = {
            { position.x - scale.x / 2.0f, position.y - probeOffsetY },
            { position.x - scale.x / 2.0f, position.y + probeOffsetY }
        };

        for (const Vector2& probe : probes) {
            if (map->isSolidTileAt(probe, &xOverlap, &yOverlap)) {
                collided = true;
                if (xOverlap > maxOverlap) maxOverlap = xOverlap;
            }
        }
        if (collided) {
            position.x += maxOverlap;
            velocity.x = 0.0f;
            collisionFlags |= COLLIDE_LEFT;
        }
}
}
void Entity::doAI(float dt, Entity* player, Map* map, Entity* platforms, int platformCount) {
    if (aiType == WANDERER) {
        wanderTimer += dt;
        velocity.x = wanderDir * speed * 0.6f;
        if (wanderTimer > 1.8f) {
            wanderDir *= -1.0f;
            wanderTimer = 0;
        }
    } else if (aiType == FOLLOWER) {
        if (player) {
            float dx = player->position.x - position.x;
            if (fabsf(dx) > 5.0f)
                velocity.x = (dx > 0 ? 1.0f : -1.0f) * speed * 0.5f;
            else
                velocity.x = 0;
        }
    } else if (aiType == JUMPER) {
        wanderTimer += dt;
        jumpTimer +=dt;
        velocity.x = wanderDir * speed * 0.5f;
        if (wanderTimer > 2.0f) {
            wanderDir *= -1.0f;
            wanderTimer = 0;
        }
        if (jumpTimer > 1.5f && isOnGround()) {
            velocity.y = -jumpPower * 0.85f;
            jumpTimer = 0;
        }
    } 
    else if (aiType == FLYER) {
        if (map) {
            float topLimit = map->getTopBoundary() + scale.y * 2.0f;
            float bottomLimit = map->getBottomBoundary() - scale.y * 5.0f;
            if (position.y < topLimit) verticalDir = 1.0f;
            if (position.y > bottomLimit) verticalDir = -1.0f;
        }
        velocity.x = 0.0f;
        velocity.y = verticalDir * speed * 0.55f;
    } 
    else if (aiType == DIAGONALER) {
        if (map) {
            float leftLimit = map->getLeftBoundary() + scale.x;
            float rightLimit = map->getRightBoundary() - scale.x;
            float topLimit = map->getTopBoundary() + scale.y * 2.0f;
            float bottomLimit = map->getBottomBoundary() - scale.y * 5.0f;
            if (position.x < leftLimit) wanderDir = 1.0f;
            if (position.x > rightLimit) wanderDir = -1.0f;
            if (position.y < topLimit) verticalDir = 1.0f;
            if (position.y > bottomLimit) verticalDir = -1.0f;
        }
        velocity.x = wanderDir * speed * 0.5f;
        velocity.y = verticalDir * speed * 0.4f;
    }
}
void Entity::update(float dt,Entity* player, Map* map, Entity* platforms, int platformCount) {
    if (!active){
        return;
    }
    if (type == ENEMY) doAI(dt, player, map, platforms, platformCount);
    collisionFlags =COLLIDE_NONE;
    if (velocity.x > 1.0f) facingRight = true;
    else if (velocity.x< -1.0f) facingRight = false;

    velocity.x += acceleration.x * dt;
    velocity.y += acceleration.y * dt;
    if (velocity.y > 1200.0f) velocity.y = 1200.0f;
    bool useTerrainCollision = !(type == ENEMY && (aiType == FLYER || aiType == DIAGONALER));
    position.y += velocity.y * dt;

    if (useTerrainCollision && map) {
        resolveMapCollisionsY(map);
    }
    if (useTerrainCollision && platforms && platformCount > 0)
        resolveCollisions(platforms, platformCount);

    position.x += velocity.x * dt;

    if (useTerrainCollision && map) {
        resolveMapCollisionsX(map);
    }
    if (useTerrainCollision && platforms && platformCount > 0)
        resolveCollisions(platforms, platformCount);

    if (type == PLAYER) {
        const bool airborne = !isOnGround();
        const bool moving = fabsf(velocity.x) > 5.0f;

        if (airborne) {
            animationFrame = 0;
            animationTimer = 0.0f;
        } else if (moving) {
            constexpr int WALK_FRAMES = 18;
            animationTimer += dt;
            if (animationTimer >= 0.08f) {
                animationFrame = (animationFrame + 1) % WALK_FRAMES;
                animationTimer = 0.0f;
            }
        } else {
            constexpr int IDLE_FRAMES = 10;
            animationTimer += dt;
            if (animationTimer >= 0.12f) {
                animationFrame = (animationFrame + 1) % IDLE_FRAMES;
                animationTimer = 0.0f;
            }
       }
}
}
void Entity::render() const{
    if (!active){
        return;}
    if (type == PLAYER && gPlayerIdleTexture.id > 0 && gPlayerWalkTexture.id > 0) {
        const bool airborne = !isOnGround();
        const bool moving = fabsf(velocity.x) > 5.0f;

        Texture2D texture = moving && !airborne ? gPlayerWalkTexture : gPlayerIdleTexture;
        Rectangle source = {0, 0, 0, 0};

        if (airborne) {
            source = {0.0f, 0.0f, 46.0f, 55.0f};
        } else if (moving) {
            constexpr int WALK_COLUMNS = 3;
            constexpr float WALK_FRAME_WIDTH = 60.0f;
            constexpr float WALK_FRAME_HEIGHT = 58.0f;
            int col = animationFrame % WALK_COLUMNS;
            int row = animationFrame / WALK_COLUMNS;
            source = {
                col * WALK_FRAME_WIDTH,
                row * WALK_FRAME_HEIGHT,
                WALK_FRAME_WIDTH,
                WALK_FRAME_HEIGHT
            };
        } else {
            constexpr float IDLE_FRAME_WIDTH = 46.0f;
            constexpr float IDLE_FRAME_HEIGHT = 55.0f;
            source = {
                animationFrame * IDLE_FRAME_WIDTH,
                0.0f,
                IDLE_FRAME_WIDTH,
                IDLE_FRAME_HEIGHT
            };
        }

        if (facingRight) {
            source.x += source.width;
            source.width *= -1.0f;
        }
        DrawTexturePro(
            texture, source, Rectangle{position.x, position.y + scale.y / 2.0f, scale.x, scale.y}, Vector2{scale.x / 2.0f, scale.y}, 0.0f, WHITE
        );
        return;
    }

    DrawRectangle(
        (int)(position.x - scale.x / 2.0f),
        (int)(position.y - scale.y / 2.0f),
        (int)scale.x,
        (int)scale.y,
        color
    );
}
