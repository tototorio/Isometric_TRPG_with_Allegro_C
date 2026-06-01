#ifndef RENDERER_H
#define RENDERER_H

#include "commons.h"

// ============================================
// MAIN ENTRY POINT
// ============================================
void renderFrame(void);


// ============================================
// PHASE 1: ISOMETRIC WORLD - STATIC
// ============================================
// Draws the foundation layer: floor tiles
void drawFloor(void);


// ============================================
// PHASE 2: SCREEN SPACE - DYNAMIC ENTITIES
// ============================================
// Draw static interactive objects (unchanging sprites)
void drawStaticObjects(void);

// Draw playable units with their current animation frame
void drawEntities(void);

// Draw the grid marker that follows the mouse
void drawHoverMarker(void);


// ============================================
// PHASE 3A: ISOMETRIC OVERLAYS - RANGE/SELECTION
// ============================================
// Draw all range markers (movement and attack ranges)
void drawRangeMarkers(void);


// ============================================
// PHASE 3B: SCREEN SPACE - HUD/UI
// ============================================
// Main HUD coordinator - calls all UI drawing functions
void drawHUDMenu(void);

// Draw action menu buttons and background
void drawActionMenu(int entityId);

// Draw entity info card (HP/MP bars, name)
void drawEntityCard(int entityId);


 
// Helpers

// Set isometric transform for world drawing
static inline void setWorldTransform(void) {
    al_use_transform(&game.assetManager.toIsometricMatrix);
}

// Set identity transform for screen drawing 
static inline void setScreenTransform(void) {
    al_use_transform(&game.assetManager.identityMatrix);
}

#endif 