#include "../include/renderer.h"

// ============================================
// FORWARD DECLARATIONS - Static Helpers
// ============================================
static void drawSpriteAtTile(ALLEGRO_BITMAP* sprite, int tileX, int tileY);
static void drawGridMarkerAtTile(int tileX, int tileY, ALLEGRO_COLOR color, float thickness);
static void drawRangeMarkersForList(node nodeList[], int count, ALLEGRO_COLOR color, float thickness);
static void drawButton(button* btn);

// ============================================
// MAIN RENDER ORCHESTRATION
// ============================================

void renderFrame() {
    // Clear and prepare backbuffer
    al_set_target_backbuffer(game.display);
    al_clear_to_color(al_map_rgb(222, 248, 255));
    
    // ─── PHASE 1: ISOMETRIC STATIC ───
    setWorldTransform();
    drawFloor();
    
    // ─── PHASE 2: SCREEN SPACE DYNAMIC ───
    setScreenTransform();
    drawStaticObjects();
    drawEntities();
    
    
    // ─── PHASE 3A: ISOMETRIC OVERLAYS ───
    setWorldTransform();
    drawHoverMarker();
    drawRangeMarkers();
    
    // ─── PHASE 3B: SCREEN SPACE UI ───
    setScreenTransform();
    drawHUDMenu();
    
    // Finalize
    al_flip_display();
}


// Isometric floor drawing

void drawFloor() {
    for (int j = 0; j < MAP_TILE_HEIGHT; j++) {
        for (int i = 0; i < MAP_TILE_LENGHT; i++) {
            ALLEGRO_BITMAP* tileTexture = game.assetManager.tileTextures[
                game.tilemap[j][i].textureIndex
            ];
            
            if (tileTexture == NULL) {
                printf("Error: Tile texture at index %d is NULL\n", 
                       game.tilemap[j][i].textureIndex);
                continue;
            }
            
            al_draw_bitmap(tileTexture, i * TILE_PIXEL_LENGHT, j * TILE_PIXEL_HEIGHT, 0);
        }
    }
}


// Entity and object drawing

void drawStaticObjects() {
    for (int i = 0; i < game.numObjects; i++) {
        object* obj = &game.objects[i];
        
        if (obj->spriteIndex < 0 || obj->spriteIndex >= game.assetManager.numObjectSprites) {
            continue;
        }
        
        ALLEGRO_BITMAP* sprite = game.assetManager.objectSprites[obj->spriteIndex];
        drawSpriteAtTile(sprite, obj->x, obj->y);
    }
}

void drawEntities() {
    for (int i = 0; i < game.numEntities; i++) {
        entity* ent = &game.entities[i];
        
        if (ent->skinIndex < 0 || ent->skinIndex >= game.assetManager.numEntitySprites) {
            continue;
        }
        
        // Query current frame from update.c (stored in entity state)
        ALLEGRO_BITMAP* sprite = game.assetManager.entitySprites[ent->skinIndex]
                                    .frames[ent->facing][ent->skinFrame];
        
        drawSpriteAtTile(sprite, ent->x, ent->y);
    }
}

void drawHoverMarker() {
    if (game.inputManager.hoveredTileX >= 0 && game.inputManager.hoveredTileY >= 0) {
        drawGridMarkerAtTile(
            game.inputManager.hoveredTileX,
            game.inputManager.hoveredTileY,
            al_map_rgb(0, 0, 0),
            3.0f
        );
    }
}

static void drawSpriteAtTile(ALLEGRO_BITMAP* sprite, int tileX, int tileY) {
    float spriteWidth = al_get_bitmap_width(sprite);
    float spriteHeight = al_get_bitmap_height(sprite);
    
    float screenCenterX = game.tilemap[tileY][tileX].centerX;
    float screenCenterY = game.tilemap[tileY][tileX].centerY;
    
    // Position sprite to stand on the tile
    float drawX = screenCenterX - (spriteWidth / 2.0f);
    float drawY = screenCenterY + (TILE_PIXEL_HEIGHT / 3.1f) - spriteHeight;
    
    al_draw_bitmap(sprite, drawX, drawY, 0);
}


// ============================================
// PHASE 3A: RANGE MARKERS (ISOMETRIC)
// ============================================

void drawRangeMarkers() {
    // Movement range markers

    if (game.hudState == HUD_MOVEMENT_MODE){
        drawRangeMarkersForList(
            game.turnManager.movementNodes,
            game.turnManager.numMovementNodes,
            al_map_rgb(100, 200, 100),  // Green for movement
            2.0f
        );
    }
    
    else if (game.hudState == HUD_ATTACK_MODE){
        // Attack range markers
        drawRangeMarkersForList(
            game.turnManager.attackNodes,
            game.turnManager.numAttackNodes,
            al_map_rgb(200, 100, 100),  // Red for attack
            2.0f
        );
    }
    
    
}

static void drawRangeMarkersForList(node nodeList[], int count, ALLEGRO_COLOR color, float thickness) {
    for (int i = 0; i < count; i++) {
        drawGridMarkerAtTile(
            nodeList[i].x,
            nodeList[i].y,
            color,
            thickness
        );
    }
}

static void drawGridMarkerAtTile(int tileX, int tileY, ALLEGRO_COLOR color, float thickness) {
    float x = tileX * TILE_PIXEL_LENGHT;
    float y = tileY * TILE_PIXEL_HEIGHT;
    
    al_draw_rectangle(x, y, x + TILE_PIXEL_LENGHT, y + TILE_PIXEL_HEIGHT, color, thickness);
}


// ============================================
// PHASE 3B: HUD MENU (SCREEN SPACE)
// ============================================

void drawHUDMenu() {
    // Get selected entity info
    int selTileX = game.inputManager.selectedTileX;
    int selTileY = game.inputManager.selectedTileY;
    UserType selectedTypeUser = game.tilemap[selTileY][selTileX].userType;
    int idSelected = game.tilemap[selTileY][selTileX].idUser;
    
    // Draw based on current HUD state
    if (game.hudState != HUD_NONE){
        drawActionMenu(game.turnManager.entityTurnID);
    }
    
    // Draw entity info card if available
    if (selectedTypeUser == TILE_USER_ENTITY) {
        drawEntityCard(idSelected);
    }
}

void drawActionMenu(int entityId) {
    (void)entityId;  // Parameter not used in current implementation
    ALLEGRO_BITMAP* menu = game.assetManager.hudAssets[0];
    
    int screenX = SCREEN_PIXEL_WIDTH;
    int screenY = SCREEN_PIXEL_HEIGHT;
    
    // Draw menu background
    al_draw_bitmap(menu, 15, SCREEN_PIXEL_HEIGHT * 0.76, 0);
    al_draw_line(screenX * 0.124, screenY * 0.79, screenX * 0.124, screenY * 0.96, 
                 al_map_rgb(255, 255, 255), 2);
    
    // Draw action buttons
    for (int i = 0; i < 4; i++) {
        button* btn = &game.buttons[i];
        if (btn->beingDisplayed) {
            drawButton(btn);
        }
    }
}

static void drawButton(button* btn) {
    ALLEGRO_FONT* font = game.assetManager.fonts[0];
    ALLEGRO_COLOR buttonColor = al_map_rgb(100, 100, 150);
    ALLEGRO_COLOR availibleBtnColor = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR consumedBtnColor = al_map_rgb(112, 112, 112);
    
    // Draw button background   
    al_draw_filled_rectangle(btn->x, btn->y, btn->x + btn->width, btn->y + btn->height, 
                             buttonColor);
    
    // Draw button border
    al_draw_rectangle(btn->x, btn->y, btn->x + btn->width, btn->y + btn->height,
                     al_map_rgb(255, 255, 255), 2);
    
    // Draw centered text
    int textWidth = al_get_text_width(font, btn->label);
    int textHeight = al_get_font_line_height(font);
    
    float textX = btn->x + (btn->width - textWidth) / 2.0f;
    float textY = btn->y + (btn->height - textHeight) / 2.0f;
    
    if (btn->type == BUTTON_ACTION_MENU_MOVE && game.turnManager.movementConsumed){
        al_draw_text(font, consumedBtnColor, textX, textY, ALLEGRO_ALIGN_LEFT, btn->label);
    }
    else if (btn->type == BUTTON_ACTION_MENU_ATTACK && game.turnManager.attackConsumed){
        al_draw_text(font, consumedBtnColor, textX, textY, ALLEGRO_ALIGN_LEFT, btn->label);
    }
    else {
        al_draw_text(font, availibleBtnColor, textX, textY, ALLEGRO_ALIGN_LEFT, btn->label);
    }
}

void drawEntityCard(int entityId) {
    entity* ent = &game.entities[entityId];
    ALLEGRO_FONT* font = game.assetManager.fonts[0];

    int screenWidth = SCREEN_PIXEL_WIDTH;
    
    // Card dimensions and padding
    int cardPadding = 15;          // Distance from screen edges
    int cardWidth = 400;           // Width of info card
    int cardHeight = 185;          // Height of info card
    
    // Internal card spacing
    int cardInnerPadding = 25;     // Text padding inside card
    int textLineHeight = 25;       // Spacing between text elements
    int barHeight = 25;            // Height of HP/MP bars
    int barWidth = 250;            // Width of progress bars
    
    // Calculate positions based on team
    int cardX = (ent->team == 1) ? cardPadding : (screenWidth - cardWidth - cardPadding);
    int cardY = cardPadding;
    
    // Bar positions relative to card
    int hpBarY = cardY + cardInnerPadding + textLineHeight;
    int mpBarY = hpBarY + textLineHeight + barHeight;
    int barStartX = cardX + cardInnerPadding + 35;  // Offset for label
    
    // Calculate health/mana percentages
    float percentageHP = (float)ent->HP / ent->maxHP;
    float percentageMP = (float)ent->MP / ent->maxMP;

    char lineBuffer[200];
    sprintf(lineBuffer, "%s", ent->name);

    // === DRAW CARD BACKGROUND ===
    al_draw_filled_rectangle(
        cardX, cardY, 
        cardX + cardWidth, cardY + cardHeight, 
        al_map_rgb(116, 135, 161)
    );

    // === DRAW ENTITY NAME ===
    al_draw_text(
        font, al_map_rgb(0, 0, 0), 
        cardX + cardInnerPadding, cardY + cardInnerPadding, 
        ALLEGRO_ALIGN_LEFT, lineBuffer
    );

    // === DRAW HP BAR ===
    al_draw_text(
        font, al_map_rgb(0, 0, 0), 
        cardX + cardInnerPadding, hpBarY, 
        ALLEGRO_ALIGN_LEFT, "HP"
    );
    // Background (black)
    al_draw_filled_rectangle(
        barStartX, hpBarY, 
        barStartX + barWidth, hpBarY + barHeight, 
        al_map_rgb(0, 0, 0)
    );
    // Fill (red)
    al_draw_filled_rectangle(
        barStartX, hpBarY, 
        barStartX + (barWidth * percentageHP), hpBarY + barHeight, 
        al_map_rgb(224, 0, 0)
    );

    // === DRAW MP BAR ===
    al_draw_text(
        font, al_map_rgb(0, 0, 0), 
        cardX + cardInnerPadding, mpBarY, 
        ALLEGRO_ALIGN_LEFT, "MP"
    );
    // Background (black)
    al_draw_filled_rectangle(
        barStartX, mpBarY, 
        barStartX + barWidth, mpBarY + barHeight, 
        al_map_rgb(0, 0, 0)
    );
    // Fill (blue)
    al_draw_filled_rectangle(
        barStartX, mpBarY, 
        barStartX + (barWidth * percentageMP), mpBarY + barHeight, 
        al_map_rgb(0, 123, 224)
    );
}
