#ifndef SETUP_H
#define SETUP_H

//Inclusions
#include "commons.h"
#include "update.h"

//Function prototypes
void init();
void initializeAllegro();
void initializeGameVariables();
void createMatrices();
void createTilemap();
// Asset loading functions
void loadMapAssets(char* map);
void loadMapDisposition(char* filename, AssetDiccionaryNode objectDiccionary[MAX_SPRITES]);
// Entities
void loadEntityStats(char* filename);
void loadEntitySprites(char* filename);
void loadSkinFrames (int index);
// Objects
void loadInteractiveObjectStats(char* filename);
void loadObjectSprites(char* filename, AssetDiccionaryNode objectDiccionary[MAX_SPRITES]);
// Tile textures
void loadTileTextures(char* filename);

void createHUD();
void createButton(
    int id, ButtonType type, char* label, 
    float x, float y, float width, float height
);

void startQueue();

#endif