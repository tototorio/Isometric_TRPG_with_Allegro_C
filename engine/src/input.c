#include "../include/input.h"
#include "../include/update.h"

void handleClick(float x, float y){

    //Store click in game context
    game.inputManager.clickX = x;
    game.inputManager.clickY = y;

    // Debug
    printf("Click at pixel coordinates: (%.2f, %.2f)\n", x, y);

    //Get cartesian coordinates to communicate with tilemap
    float cartX = x;
    float cartY = y;
    al_transform_coordinates(&game.assetManager.reverseIsometricMatrix, &cartX, &cartY);

    // Inside map case, check tile clicks
    if (cartX > 0 && cartX < MAP_PIXEL_LENGHT && cartY > 0 && cartY < MAP_PIXEL_HEIGHT){
        //Get tile coordinates
        int tileX = (int)(cartX / TILE_PIXEL_LENGHT);
        int tileY = (int)(cartY / TILE_PIXEL_HEIGHT);

        //Store selected tile in game context
        game.inputManager.selectedTileX = tileX;
        game.inputManager.selectedTileY = tileY;

        if (game.hudState == HUD_MOVEMENT_MODE){
            handleMovementClick(tileX, tileY);
        }
        else if (game.hudState == HUD_ATTACK_MODE){
            handleAttackClick(tileX, tileY);
        }
    }
    // Outside map case, check buttons clicks
    else {
        for (int i = 0; i < game.numButtons; i++){
            button* btn = &game.buttons[i];

            if (x >= btn->x && x <= (btn->x + btn->width) && y >= btn->y && y <= (btn->y + btn->height)){
                if (!btn->beingDisplayed){
                    printf("Button [%d] not currently being displayed.\n", btn->id);
                    return;
                }
                if (btn->type == BUTTON_ACTION_MENU_MOVE && game.turnManager.movementConsumed){
                    printf("Movement button ignored. Movement already consumed this turn.\n");
                    return;
                }
                if (btn->type == BUTTON_ACTION_MENU_ATTACK && game.turnManager.attackConsumed){
                    printf("Attack button ignored. Attack already consumed this turn.\n");
                    return;
                }
                
                // Debug
                printf("Button click detected: Type %d. HUD state: %d\n", btn->type, game.hudState);
                updateHUD(btn->type);
                return;
            }
        }
    }

} 

void handleMovementClick(int tileX, int tileY){
    // Debug
    printf("Handling movement click on tile (%d, %d)\n", tileX, tileY);
    
    // Check if clicked tile is a valid movement option
    for (int i = 0; i < game.turnManager.numMovementNodes; i++){
        node* currNode = &game.turnManager.movementNodes[i];
        if (currNode->x == tileX && currNode->y == tileY){
            // Debug
            printf("Tile (%d, %d) is a valid movement option. Executing move.\n", tileX, tileY);
            
            moveEntityToTile(game.turnManager.entityTurnID, tileX, tileY);
            return;
        }

    }
}

void handleAttackClick(int tileX, int tileY){
    printf("\n[ATTACK] Click on tile (%d, %d)\n", tileX, tileY);
    printf("[ATTACK] Checking %d valid attack nodes...\n", game.turnManager.numAttackNodes);
    
    tile targetTile = game.tilemap[tileY][tileX];

    // Check if clicked tile is a valid attack option
    for (int i = 0; i < game.turnManager.numAttackNodes; i++){
        node* currNode = &game.turnManager.attackNodes[i];
        if (currNode->x == tileX && currNode->y == tileY){
            printf("[ATTACK] Tile IN RANGE. Checking target...\n");
           
            // We make sure the target is occupied by an entity
            if (targetTile.userType == TILE_USER_ENTITY){
                printf("[ATTACK] Target tile has entity\n");
                
                entity* attacker = &game.entities[game.turnManager.entityTurnID];
                entity* target = &game.entities[targetTile.idUser];
                printf("[ATTACK] Target: %s (ID: %d) | Team: %d\n", target->type, target->id, target->team);
                
                // Check if the entity is an enemy
                if (target->team == 2){
                    printf("[ATTACK] Target is ENEMY. Attack VALID!\n");

                    attackEntity(game.turnManager.entityTurnID, target->id);
                    return;
                } else {
                    printf("[ATTACK] Target is NOT enemy (team %d). Ignoring.\n", target->team);
                }
            } else {
                printf("[ATTACK] Tile is empty or not entity (type: %d). Ignoring.\n", targetTile.userType);
            }
            return;
        }

    }
}

void handleMouseMove(float x, float y){

    float cartX = x;
    float cartY = y;
    al_transform_coordinates(&game.assetManager.reverseIsometricMatrix, &cartX, &cartY);

    if (cartX > 0 && cartX < MAP_PIXEL_LENGHT && cartY > 0 && cartY < MAP_PIXEL_HEIGHT){
        int tileX = (int)(cartX / TILE_PIXEL_LENGHT);
        int tileY = (int)(cartY / TILE_PIXEL_HEIGHT);

        game.inputManager.hoveredTileX = tileX;
        game.inputManager.hoveredTileY = tileY;

    } else {
        game.inputManager.hoveredTileX = -1;
        game.inputManager.hoveredTileY = -1;
    }
}