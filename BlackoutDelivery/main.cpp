#include "raylib.h"
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "MenuScene.h"


// CONSTANTS

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr float PLAYER_WIDTH = 32.0;
constexpr float PLAYER_HEIGHT = 32.0;
constexpr float PLAYER_SPEED = 80.0f;
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

constexpr int RAIN_COUNT = 180;
constexpr float RAIN_WIND = -120.0f;

constexpr float SCALE_X = SCREEN_WIDTH / 880.0f;
constexpr float SCALE_Y = SCREEN_HEIGHT / 495.0f;
constexpr float FLASHLIGHT_R = 0.09f;
constexpr float DELIVERY_TIME = 60.0f;

constexpr float NPC_SIZE = 30.0f;
constexpr float WALKER_SPEED = 100.0f;
constexpr float DOG_SPEED = 100.0f;
constexpr float RIVAL_SPEED = 100.0f;


constexpr float TILE_SIZE = 10.0f;
constexpr int MAP_COLS = 128;
constexpr int MAP_ROWS = 72;

enum SceneID {MENU, LEVEL1, WIN, LOSE};

constexpr Rectangle DELIVERY_HOME = {14.0, 14.0, 69.0, 46.0};

// BUILDINGS

static const std::vector<Rectangle> buildings = {
    {  31, 339, 70,  73 },
    { 165, 383, 71,  73 },
    { 280, 418, 74,  78 },
    { 413, 349, 69, 105 },
    { 539, 372, 72,  84 },
    { 682, 394, 73,  86 },
    { 798, 338, 71, 119 },
    {  16, 190, 72,  98 },
    { 102, 254, 71,  79 },
    { 199, 236, 76,  96 },
    { 305, 286, 71,  80 },
    { 413, 236, 71,  72 },
    { 518, 202, 75, 105 },
    { 608, 131, 74, 109 },
    { 646, 279, 72,  72 },
    { 761, 107, 74,  85 },
    { 779, 246, 71,  70 },
    {  33,  98, 72,  70 },
    { 128, 140, 73,  73 },
    { 314, 166, 72,  73 },
    { 609, 132, 72, 104 },
    { 761, 107, 73,  85 },
    { 134,  10, 72,  71 },
    { 218,  31, 73,  72 },
    { 348,  44, 73,  76 },
    { 484,  60, 70,  87 },
    { 653,  11, 74,  72 },
    { 761,   5, 72,  92 },
};

static Rectangle toScreen(const Rectangle &b){
    return {b.x * SCALE_X, b.y * SCALE_Y, b.width * SCALE_X, b.height * SCALE_Y};

}

static bool blocked(Vector2 p){
    Rectangle pr = {p.x, p.y, PLAYER_WIDTH, PLAYER_HEIGHT};
    for (const Rectangle &b : buildings){
        if (CheckCollisionRecs(pr, toScreen(b))){
            return true;
        }
    }
    return false;

}

struct Raindrop{
    Vector2 position;
    float speed; 
    float length;
};

int main(){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Blackout Delivery");
    SetTargetFPS(60);
    InitAudioDevice();

    // LOADING

    Texture2D city = LoadTexture("assets/map.png");
    Texture2D pizza = LoadTexture("assets/pizza.png");
    Shader flashlight = LoadShader("shaders/vertex.glsl", "shaders/fragment.glsl");
    Music tick = LoadMusicStream("assets/sfx/tick.mp3");
    Music storm = LoadMusicStream ("assets/sfx/storm.mp3");
    Sound doorbell = LoadSound("assets/sfx/doorbell.mp3");
    storm.looping = true;
    tick.looping = true;

    // SHADER SETUP

    int locLight = GetShaderLocation(flashlight, "lightPosition");
    int locScreen = GetShaderLocation(flashlight, "screenSize");
    int locRadius = GetShaderLocation(flashlight, "lightRadius");
    Vector2 screenPx {(float) GetRenderWidth(), (float) GetRenderHeight()};
    float r = FLASHLIGHT_R;
    SetShaderValue(flashlight, locScreen, &screenPx, SHADER_UNIFORM_VEC2);
    SetShaderValue(flashlight, locRadius, &r, SHADER_UNIFORM_FLOAT);

    SceneID scene = MENU;
    Vector2 spawn ={60.0f, SCREEN_HEIGHT- 80.f};
    std::map<Direction, std::vector<int>> atlas = {
        {DOWN, {0,1,2,3}}, {LEFT, {4,5,6,7}}, {RIGHT, {8,9,10,11}}, {UP, {12,13,14,15}},
    };

    // PLAYER

    Entity *player = new Entity(spawn, {PLAYER_WIDTH, PLAYER_HEIGHT},
        "assets/sprites/cyclist.png", ATLAS, {4,4}, atlas, PLAYER);
    player->setSpeed(PLAYER_SPEED);
    player->setColliderDimensions({28.0f, 28.0f});
    std::map<Direction, std::vector<int>> driverAtlas = {
        {DOWN, {0,1,2,3}}, {LEFT, {4,5,6,7}}, {RIGHT, {8,9,10,11}}, {UP, {12,13,14,15}},
    };

    // PATROL WAYPOINTS

    Vector2 walkerA = { 257*SCALE_X, 341*SCALE_Y};
    Vector2 walkerB = {257*SCALE_X, 490 *SCALE_Y};
    Vector2 dogA = {715 *SCALE_X, 92*SCALE_Y};
    Vector2 dogB = {715 *SCALE_X, 264*SCALE_Y};
    Vector2 rivalA = {240 *SCALE_X, 112*SCALE_Y};
    Vector2 rivalB = {240*SCALE_X, 224*SCALE_Y};
    int walkerTarget = 1, dogTarget = 1, rivalTarget = 1;

    // NPC ENTITIES

    Entity *walker = new Entity(walkerA, {NPC_SIZE, NPC_SIZE},
    "assets/sprites/walker.png", ATLAS, {4,4}, driverAtlas, PLAYER);
    walker->setSpeed(WALKER_SPEED);
    Entity *dog= new Entity(dogA, {NPC_SIZE, NPC_SIZE},
    "assets/sprites/dog.png", ATLAS, {4,4}, driverAtlas, PLAYER);
    dog->setSpeed(DOG_SPEED);
    Entity *rival = new Entity(rivalA, {NPC_SIZE, NPC_SIZE},
    "assets/sprites/rival.png", ATLAS, {4,4}, driverAtlas, PLAYER);
    rival->setSpeed(RIVAL_SPEED);

    // MENU SCENE 

    MenuScene *menuScene = new MenuScene();
    menuScene->initialise();

    // LEVEL DATA / TILES
    unsigned int *levelData = new unsigned int[MAP_COLS * MAP_ROWS]();
    for (int r = 0; r<MAP_ROWS; r++){
        for (int c = 0; c < MAP_COLS; c++){
            Rectangle tile = {c * TILE_SIZE, r * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            for (const Rectangle &b: buildings){
                if (CheckCollisionRecs(tile, toScreen(b))){
                    levelData[r* MAP_COLS + c] = 1;
                    break;
                }
            }
        }
    }

    // MAP BORDERS

    for (int c = 0 ; c< MAP_COLS; c++){
        levelData[c] = 1;
        levelData[(MAP_ROWS-1) * MAP_COLS+c] = 1;
    }
    for (int r = 0 ; r< MAP_ROWS; r++){
        levelData[r * MAP_COLS] = 1;
        levelData[r* MAP_COLS + (MAP_COLS-1)] = 1;
    }

    // MAPS

    Vector2 mapOrigin = {SCREEN_WIDTH/ 2.0f, SCREEN_HEIGHT / 2.0};
    Map *map = new Map(MAP_COLS, MAP_ROWS, levelData, "assets/tile.png", 
    TILE_SIZE, 1, 1, mapOrigin);

    // PIZZA

    float timer = DELIVERY_TIME;
    bool hasPizza = false;
    Rectangle pizzaRect = {850*SCALE_X, 9*SCALE_Y, 32, 32};
    
    // RAIN INITIALISATION
    std::vector<Raindrop> rain;
    for (int i = 0; i < RAIN_COUNT; i++){
        Raindrop drop;
        drop.position = {(float)GetRandomValue(0, SCREEN_WIDTH),
        (float)GetRandomValue(-SCREEN_HEIGHT, SCREEN_HEIGHT)};
    drop.speed = (float)GetRandomValue(450, 750);
    drop.length = (float)GetRandomValue(10, 20);
    rain.push_back(drop);
    }
    
    float prevT=(float)GetTime();
    float acc = 0.0f;

    while (!WindowShouldClose()){
        float now = (float)GetTime();
        float dt = now-prevT;
        prevT = now;
        if (dt > 0.1f){
            dt = 0.1f;
        }

    // RAIN UPDATE

    for (Raindrop &drop : rain){
        drop.position.y += drop.speed * dt;
        drop.position.x += RAIN_WIND * dt;
        if (drop.position.y > SCREEN_HEIGHT || drop.position.x <-50){
        drop.position.x = (float)GetRandomValue(0,SCREEN_WIDTH);
        drop.position.y = (float)GetRandomValue(-100, 0);
        drop.length = (float)GetRandomValue(10, 20);
    }
    
    }
    if (IsKeyPressed(KEY_ESCAPE)){
        break;
    }

    // MENU INPUT

    if (scene == MENU){
        menuScene->update(dt);
        if (menuScene->getState().nextSceneID == 1){
            scene = LEVEL1;
            player->setPosition(spawn);
            timer = DELIVERY_TIME;
            hasPizza=false;
            PlayMusicStream(tick);
            PlayMusicStream(storm);
            menuScene->initialise();

        }
        
        
        
    }

    // RETRY INPUT
    if ((scene == WIN || scene == LOSE) && IsKeyPressed(KEY_ENTER)){
        scene = LEVEL1;
            player->setPosition(spawn);
            timer = DELIVERY_TIME;
            hasPizza = false;
            walker->setPosition(walkerA);
            dog->setPosition(dogA);
            rival->setPosition(rivalA);
            walkerTarget=dogTarget=rivalTarget=1;
            PlayMusicStream(tick);
            PlayMusicStream(storm);
            menuScene->initialise();
    }
    acc += dt;

    // MUSIC UPDATE

    UpdateMusicStream(tick);
    UpdateMusicStream(storm);

    // FIXED TIMESTEP
    while (acc >= FIXED_TIMESTEP){
        player->resetMovement();
        if (scene == LEVEL1){
            if (IsKeyDown(KEY_W)){
                player->moveUp();
            
            }
            if (IsKeyDown(KEY_A)){
                player->moveLeft();
            }
            if (IsKeyDown(KEY_S)){
                player->moveDown();
            }
            if (IsKeyDown(KEY_D)){
                player->moveRight();
            
            }
            Vector2 mv= player->getMovement();
            if (mv.x>0){
                player->setDirection(RIGHT);

            }
            else if (mv.x < 0){
                player->setDirection(LEFT);
            }
            else if (mv.y > 0){
                player->setDirection(DOWN);
            }
            else if (mv.y < 0){
                player->setDirection(UP);
            }
            player->normaliseMovement();
            player->update(FIXED_TIMESTEP, player, map, nullptr, 0);
            
            // PIZZA PICKUP

            Rectangle playerRect = {
                player->getPosition().x - PLAYER_WIDTH/2,
                player->getPosition().y - PLAYER_HEIGHT/2,
                PLAYER_WIDTH, PLAYER_HEIGHT
            };
            if (!hasPizza && CheckCollisionRecs(playerRect, pizzaRect)){
                hasPizza= true;
                timer+= 60.0f;

            }
     
            
            timer-= FIXED_TIMESTEP;

            // AI PATROL

            auto patrol = [&](Entity *e, Vector2 a, Vector2 b, int &target){
                Vector2 goal = (target ==1) ? b : a;
                Vector2 pos = e->getPosition();
                float dx = goal.x - pos.x, dy = goal.y-pos.y;
                e->resetMovement();
                if (sqrt(dx*dx + dy*dy) < 20.0f){
                    target = (target == 1) ? 0: 1;
                    goal = (target == 1) ? b: a;
                    return;
                }
                if (fabs(dx) > fabs(dy)){
                    if (dx > 0){
                        e->moveRight();

                    }
                    else{
                        e->moveLeft();
                    }
                }
                else{
                    if (dy > 0 ){
                        e->moveDown();

                    }
                    else{
                        e->moveUp();
                    }
                 }
                e->normaliseMovement();
                e->update(FIXED_TIMESTEP, player, map, nullptr, 0);
                
            
            };

            patrol(walker, walkerA, walkerB, walkerTarget);
            patrol(dog, dogA, dogB, dogTarget);
            patrol(rival, rivalA, rivalB, rivalTarget);
            
            // NPC HIT CHECK

            Vector2 pp = player->getPosition();
            Entity *npcs[3] = {walker, dog, rival};
            for (Entity *n : npcs){
                Vector2 np = n->getPosition();
                float ex = pp.x - np.x, ey = pp.y -np.y;
                if (sqrtf(ex*ex +ey*ey)<NPC_SIZE){
                    scene =LOSE;
                    StopMusicStream(tick);
                    StopMusicStream(storm);

                }
            }

        
    
        // WIN CHECK

        Rectangle pr = {player->getPosition().x- PLAYER_WIDTH/2,
                        player->getPosition().y-PLAYER_HEIGHT/2,
                        PLAYER_WIDTH, PLAYER_HEIGHT};
        Rectangle goal = toScreen(DELIVERY_HOME);
        if (CheckCollisionRecs(pr, goal) && hasPizza){
            StopMusicStream(tick);
            StopMusicStream(storm);
            PlaySound(doorbell);
            scene = WIN;
            
        }

        // LOSE CHECK

        if (timer<= 0.0f){
            scene = LOSE;
            StopMusicStream(tick);
            StopMusicStream(storm);
        }
    }
    
        acc -= FIXED_TIMESTEP;
    }
    
    
    // RENDER 

    BeginDrawing();
    ClearBackground(BLACK);

    // RENDER MENU

    if (scene == MENU){
        menuScene->render();
    }

    // RENDER LEVEL

    else if (scene == LEVEL1){
        Vector2 lightCenter={
            player->getPosition().x / SCREEN_WIDTH,
            player->getPosition().y / SCREEN_HEIGHT
        };
    SetShaderValue(flashlight, locLight, &lightCenter, SHADER_UNIFORM_VEC2);
    BeginShaderMode(flashlight);

    // RENDER CITY

    DrawTexturePro(city, {0,0, (float)city.width, (float)city.height},
    {0,0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
    {0,0}, 0.0f, WHITE);
    

    
    player->render();

    // RENDER PIZZA

    if (!hasPizza){
        DrawTexturePro(pizza, {0,0, (float)pizza.width, (float)pizza.height}, pizzaRect, {0,0}, 0.0f, WHITE);
    }

    // RENDER NPCs

    walker->render();
    dog->render();
    rival->render();
        // RENDER RAIN
        for (const Raindrop &drop : rain){
        DrawLineEx(
            drop.position, {drop.position.x - 4, drop.position.y + drop.length},
            1.5f, {120, 160, 200, 130}
    );
}
    EndShaderMode();
    //for (const Rectangle &b: buildings){
    //    Rectangle r = toScreen(b);
    //    DrawRectangleLines((int)r.x, (int)r.y, (int)r.width, (int)r.height, RED);
    //}

    // RENDER UI

    DrawText(TextFormat("TIME %02d", (int)timer), 20, 20, 32, RED);
    DrawText(hasPizza ? "PIZZA: COLLECTED" : "PIZZA: UNCOLLECTED", 20, 60, 24, hasPizza ? GREEN: RED);
    }

    // RENDER WIN

    else if (scene == WIN){
        DrawText("DELIVERED!", 480, 300, 56, GREEN);
        DrawText("ENTER for menu", 490, 380, 24, GRAY);
    }

    // RENDER LOSE

    else if (scene == LOSE){
        DrawText("FAILED TO DELIVER!", 400, 300, 56, RED);
        DrawText("ENTER for menu", 490, 380, 24, GRAY);
        
    }
    EndDrawing();
}
    // CLEANUP
    
    delete player;
    delete walker;
    delete dog;
    delete rival;
    delete menuScene;
    UnloadShader(flashlight);
    UnloadTexture(city);
    UnloadTexture(pizza);
    UnloadMusicStream(tick);
    UnloadMusicStream(storm);
    UnloadSound(doorbell);
    CloseAudioDevice();
    delete map;
    delete[] levelData;
    CloseWindow();
    return 0;


}
// WIN COORDS : X: 14 Y: 13, W: 69 H: 46
// Left to right bottom to top
// X:  31   Y:      339, W=73, H=78
// X:  165   Y:      383.   W=71, H=73
// X:  280   Y:      418. W = 74, H = 78
// X:   413  Y:      349. W = 69, H = 105
// X:    539  Y:      372 W = 72, H = 84
// X:     682 Y:      394 W = 73, H = 86
// X:     798 Y:      338 W = 71, H = 119
// X:     16  Y:     190  W = 72, H=98
// X:    102  Y:     254 W = 71, H = 79
// X:    199  Y:      236 W = 76, H = 96
// X:    305  Y:      286 W = 75, H = 81
// X:     413  Y:      236 H = 72, W = 71
// X:      518 Y:      202 W = 75, H= 105
// X:     608 Y:      131 W = 74, H = 109
// X:     646  Y:      279 W = 72, H = 72
// X:      761 Y:      107 W = 74, H = 85
// X:      779 Y:      246 W = 71, H = 70
// X:     33  Y:       98  W = 72, H = 70
// X:     128  Y:     140  W = 73 H = 73
// X:     314  Y:     166 W = 72, H = 73
// X:     609  Y:     132. W= 72, H= 104
// X:     761  Y:     107 W= 73, H= 85
// X:     134  Y:     10. W= 72, H= 71
// X:     218  Y:     31. W= 73, H= 72
// X:     348  Y:     44 W= 73, H= 76
// X:     484  Y:     60. W= 70, H= 87
// X:     653   Y:    11. W= 74, H= 72
// X:     761   Y:    5   W= 72, H=92
