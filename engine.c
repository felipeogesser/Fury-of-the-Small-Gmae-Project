#define MAX_OBJECTS = 100;
#define MAX_PLAYERS = 10;
struct Player {
    int id;
    char name[32];
    float spawnX;
    float spawnY;
    float dimensionX;
    float dimensionY;
    bool collision;
    int quadrant;
}

struct Player players[MAX_PLAYERS];

int create_player(char name, float spawnX, float spawnY, float dimensionX, float dimensionY, bool collision, int quadrant) {
    int id = player_count;
    objects[id].id = id;
    objects[id].name = name;
    objects[id].pointX = spawnX;
    objects[id].pointY = spawnY;
    objects[id].dimensionX = dimensionX;
    objects[id].dimensionY = dimensionY;
    objects[id].collision = collision;
    objects[id].quadrant = quadrant;
    player_count++;
    return id;
}

int player_id = create_player(lipe, 100.0f, 100.0f, 30.0f, 30.0f, true, 4);
struct Player *p = &players[player_id];


struct Object {
    int id;
    char name[32];
    float pointX;
    float pointY;
    float dimensionX;
    float dimensionY;
    bool collision;
    int quadrant;
}

struct Object objects[MAX_OBJECTS];

int create_object(char name, float pointX, float pointY, float dimensionX, float dimensionY, bool collision, int quadrant) {
    int id = object_count;
    objects[id].id = id;
    objects[id].name = name;
    objects[id].pointX = pointX;
    objects[id].pointY = pointY;
    objects[id].dimensionX = dimensionX;
    objects[id].dimensionY = dimensionY;
    objects[id].collision = collision;
    objects[id].quadrant = quadrant;
    object_count++;
    return id;
}

    
int wall_id = create_object(wallV, 600.0f, 40.0f, 30.0f, 400.0f, true, 4);
struct Object *o = &objects[wall_id];


float PlayerPositionX = p->spawnX;
float PlayerPositionY = p->spawnY;
float PlayerDimensionX = p->dimensionX;
float PlayerDimensionY = p->dimensionY;
float PlayerLimitPositionX = WindowSizeX - PlayerDimensionX;
float PlayerLimitPositionY = WindowSizeY - PlayerDimensionY;

float WallPositionX = o->pointX;
float WallPositionY = o->pointY;
float WallDimensionX = o->dimensionX;
float WallDimensionY = o->dimensionY;

float DirUp = 0.0f, DirDown = 0.0f, DirLeft = 0.0f, DirRight = 0.0f;
const float speed = 150.0f;
const float runSpeed = 2.4f;
const float invSqrt2 = 0.70710678f;
float vx = 0.0f, vy = 0.0f, vz = 0.0f;
  

float hitBoxPlayer[4][2] = {
    {PlayerPositionX, PlayerPositionY},
    {PlayerPositionX + PlayerDimensionX, PlayerPositionY},
    {PlayerPositionX, PlayerPositionY + PlayerDimensionY},
    {PlayerPositionX + PlayerDimensionX, PlayerPositionY + PlayerDimensionY}
};

float hitBoxWall[4][2] = {
    {WallPositionX, WallPositionY},
    {WallPositionX + WallDimensionX, WallPositionY},
    {WallPositionX, WallPositionY + WallDimensionY},
    {WallPositionX + WallDimensionX, WallPositionY + WallDimensionY}
};







hitBoxPlayer[0][0] = PlayerPositionX;
hitBoxPlayer[0][1] = PlayerPositionY;
hitBoxPlayer[1][0] = PlayerPositionX + PlayerDimensionX;
hitBoxPlayer[1][1] = PlayerPositionY;
hitBoxPlayer[2][0] = PlayerPositionX;
hitBoxPlayer[2][1] = PlayerPositionY + PlayerDimensionY;
hitBoxPlayer[3][0] = PlayerPositionX + PlayerDimensionX;
hitBoxPlayer[3][1] = PlayerPositionY + PlayerDimensionY;

if (vx > 0) {
    if (hitBoxPlayer[1][0] >= hitBoxWall[0][0] && hitBoxPlayer[1][0] < hitBoxWall[1][0] || hitBoxPlayer[3][1] < hitBoxWall[0][1] && hitBoxPlayer[1][1] > hitBoxWall[2][1]) {
        if (hitBoxPlayer[1][1] > hitBoxWall[2][1] || hitBoxPlayer[3][1] < hitBoxWall[0][1]) {
        } else PlayerPositionX = hitBoxWall[0][0] - PlayerDimensionX;
    }
} else

if (vx < 0) {
    if (hitBoxPlayer[0][0] <= hitBoxWall[1][0] && hitBoxPlayer[0][0] > hitBoxWall[0][0] && hitBoxPlayer[2][1] > hitBoxWall[1][1] && hitBoxPlayer[0][1] < hitBoxWall[3][1]) {
        if (hitBoxPlayer[0][1] > hitBoxWall[3][1] || hitBoxPlayer[2][1] < hitBoxWall[1][1]) {
        } else PlayerPositionX = hitBoxWall[1][0];
    }
}

if (vy > 0) {
    if (hitBoxPlayer[2][1] >= hitBoxWall[0][1] && hitBoxPlayer[2][1] < hitBoxWall[2][1] && hitBoxPlayer[3][0] > hitBoxWall[0][0] && hitBoxPlayer[2][0] < hitBoxWall[1][0]) {
        if (hitBoxPlayer[2][0] > hitBoxWall[1][0] || hitBoxPlayer[3][0] < hitBoxWall[0][0]) {
        } else PlayerPositionY = hitBoxWall[0][1] - PlayerDimensionY;
    }
} else

if (vy < 0) {
    if (hitBoxPlayer[0][1] <= hitBoxWall[2][1] && hitBoxPlayer[0][1] > hitBoxWall[0][1] && hitBoxPlayer[1][0] > hitBoxWall[2][0] && hitBoxPlayer[0][0] < hitBoxWall[3][0]) {
        if (hitBoxPlayer[0][0] > hitBoxWall[3][0] || hitBoxPlayer[1][0] < hitBoxWall[2][0]) {
        } else PlayerPositionY = hitBoxWall[2][1];
    }
}
