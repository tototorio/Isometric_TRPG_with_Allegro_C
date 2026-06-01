#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "constants.h"

typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING, // For game board screen
    GAME_STATE_END_SCREEN, // For endscreen
} GameState;

typedef enum {
    TURN_STATE_PLAYER, // For player turn
    TURN_STATE_AI, // For AI turn
    TURN_STATE_DRAWING, // For drawing and animation process
    TURN_STATE_WAITING // For menuing and other needs
} TurnState;

typedef enum {
    TILE_USER_ENTITY,
    TILE_USER_STATIC_OBJECT,
    TILE_USER_INTERACTIVE_OBJECT,
    TILE_USER_NONE
} UserType;

typedef enum {
    BUTTON_NONE, 
    BUTTON_TILE_CLICKED, // Not used for actual buttons, but to identify clicks on the map.
    BUTTON_ACTION_MENU_MOVE,
    BUTTON_ACTION_MENU_ATTACK,
    BUTTON_ACTION_MENU_SKILL,
    BUTTON_ACTION_MENU_END_TURN,
    BUTTON_MOVEMENT,
    BUTTON_NORMAL_ATTACK,
    BUTTON_SKILL_1,
    BUTTON_SKILL_2,
    BUTTON_SKILL_3,
} ButtonType;

typedef enum {
    HUD_NONE,
    HUD_ACTION_MENU,
    HUD_MOVEMENT_MODE,
    HUD_ATTACK_SUBMENU,
    HUD_ATTACK_MODE,
} HudState;

// Attack option, only relevant when HUD_ATTACK_SUBMENU
typedef enum {
    ATTACK_NONE,
    ATTACK_NORMAL,
    ATTACK_SKILL_1,
    ATTACK_SKILL_2,
    ATTACK_SKILL_3,
} AttackOption;

typedef struct {
    int id;
    bool beingDisplayed;
    ButtonType type;
    char label[50];
    float x;
    float y;
    float width;
    float height;
} button;

//Auxiliary

//Used for temporary diccionaries to translate code to index for assets.
typedef struct {

    char code[5]; //Letter plus number
    int index; //Index in the corresponding list (entities, skills or objects) 

} AssetDiccionaryNode;

typedef struct {
    
    char name[50];
    ALLEGRO_BITMAP* frames[4][MAX_IDLE_FRAMES];
    int numFrames;

} skin;

//Animation structure
//Animates either attacks/skills or movements.
typedef struct {
    //Identifier
    int type; //0: Movement, 1: Attack/skill
    
    //Frame variables
    ALLEGRO_BITMAP* frames[MAX_FRAMES]; //Stores the frames of the animation
    int numFrames; //Number of frames in the animation
    int currentFrame; //Current frame being displayed
    float frameDuration; //Duration of each frame in seconds

    //Target 
    //For attacks it will be the center of the animation,
    //For movement it will be the target position
    int x;
    int y;

    //Only movement
    float pixelPosX; //Current X position in pixels, for animations.
    float pixelPosY; //Current Y position in pixels, for animations.
    float pixelSpeedX; //Rate of change of the position X in pixels per second
    float pixelSpeedY; //Rate of change of the position Y in pixels per second
    
} animation;

//Skill structure
typedef struct {
    //Information
    int id;
    char name[50];
    
    //Stats
    int hpModifier; //Positive for healing, negative for damage
    int mpCost;
    int range;  
    int area;  
    int targetingType; //0: Enemy, 1: Ally, 2: Any

} skill;

typedef struct {
    // Fundamental information
    // For both static and interactive
    int id;
    int x;
    int y;
    int spriteIndex;
    UserType userType;
    
    // Interactive specific variables{
    char type[5]; //Defines default stats
    bool used; //Whether the object has been used or not
    //Stats to give when picked up
    int hpMod; //HP modification
    int mpMod; //MP modification
    int atkMod; //Damage of normal attacks
    int speedMod; //Affects turn cycle
    int staminaMod; //Movement range
    int rangeMod; //Range of his normal attacks

} object;

//Entity structure
typedef struct{ 
    
    int id; //Entity identifier
    int team; //1:Ally 2:Enemy
    char type[5]; //Defines default stats and skin
    
    //Map position
    int x; 
    int y;
    
    //Stats and information
    char name[50];
    int maxHP;
    int maxMP;
    int HP;
    int MP;
    int atk; //Damage of normal attacks
    int speed; //Affects turn cycle
    int stamina; //Movement range
    int range; //Range of his normal attacks

    //Skills
    int numSkills; 
    skill skillList[MAX_SPELLS]; //Almacena la información de sus hechizos

    //Turn management
    int turnGauge; //Once it reaches 100, the entity can act

    //Sprite information
    int facing; //0: North, 1: East, 2: South, 3: West
    int skinIndex;
    int skinFrame;

    //Animation handling
    animation animationQueue[MAX_ANIMATIONS];
    int currentAnimation;
    
} entity;

//Estructura usada para describir una tile
typedef struct { 
    
    float centerX; //X center in isometric pixels.
    float centerY; //Y center in isometric pixels.

    //Information
    bool isWalkable; 
    int idUser; //ID of the entity or object on the tile, -1 if empty
    UserType userType; //Whether the tile is occupied by an entity, an object or an interactive object.

    //Sprite information
    int textureIndex; //Texture index in the general texture array
    int userSpriteIndex; //With this and the user type we can determine the corresponding skin

} tile;

//Used for BFS pathfinding, represents a node in the search.
typedef struct { 
    int x;
    int y;
    int cost;
} node;

typedef struct {
    // Tile textures
    ALLEGRO_BITMAP* tileTextures[MAX_TEXTURES]; // Array to store tile textures, indexed by textureIndex
    int numTileTextures; // Number of tile textures loaded.
    // Fonts
    ALLEGRO_FONT* fonts[MAX_FONTS];
    int numFonts; // Number of fonts loaded.
    // Entity skins
    skin entitySprites[MAX_SKINS]; // Array to store entity skins, indexed by skinIndex
    int numEntitySprites; // Number of entity skins loaded.
    // Object sprites
    ALLEGRO_BITMAP* objectSprites[MAX_SPRITES]; // Array to store object sprites,
    int numObjectSprites; // Number of object sprites loaded.
    // UID
    ALLEGRO_BITMAP* hudAssets[MAX_UID_ASSETS]; // Array to store UI elements like health bars, mana bars, etc.
    int numHUDAssets; // Number of UI assets loaded.

    /// --- Tools ---
    ALLEGRO_TRANSFORM toIsometricMatrix; // Converts cartesian coordinates to isometric.
    ALLEGRO_TRANSFORM reverseIsometricMatrix; // Converts isometric coordinates back to cartesian.
    ALLEGRO_TRANSFORM identityMatrix; // Identity matrix for resetting transformations.
} assetManager;


typedef struct {
    TurnState turnState;
    node* selectedAttackNodes; // Only relevant when HUD_ATTACK_SUBMENU
    int selectedAttackListSize;
    int entityTurnID; //ID of the entity whose turn it is.
    bool movementConsumed;
    bool attackConsumed;
    

    // Arrays to store movement and attack options of the current turn holder
    node movementNodes[MAX_NODES];
    int numMovementNodes;
    
    node attackNodes[MAX_NODES];
    int numAttackNodes;

} turnManager;

typedef struct {
    float hoveredTileX; //Currently hovered tile X coordinate.
    float hoveredTileY; //Currently hovered tile Y coordinate.
    float clickX; //Last click X position in pixels.
    float clickY; //Last click Y position in pixels.
    int selectedTileX;
    int selectedTileY;
} inputManager;

//Game context structure
typedef struct {

    //Fundamental game variables.
    ALLEGRO_BITMAP* bitmap; // Contains drawing data for the map.
    ALLEGRO_TIMER* frameCounter; // Counts frames to manage animation timing.
    ALLEGRO_EVENT_QUEUE* eventQueue; // Handles input and other events.
    ALLEGRO_DISPLAY* display; // App window.

    GameState gameState;
    HudState hudState;

    // ---Map, entity and object information---

    tile tilemap[MAP_TILE_LENGHT][MAP_TILE_HEIGHT];
    entity entities[MAX_ENTITIES];
    int numEntities; //Number of entities in the game.
    object objects[MAX_OBJECTS];
    int numObjects; //Number of interactive objects in the game.
    button buttons[MAX_BUTTONS];
    int numButtons; //Number of buttons in the UI.


    // Managers

    assetManager assetManager;
    inputManager inputManager;
    turnManager turnManager;
    

} gameContext;



#endif