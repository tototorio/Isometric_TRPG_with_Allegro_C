#include "../include/update.h"

// For now it will only check if its the IA turn.
void updateGame(){
    if (game.turnManager.turnState == TURN_STATE_AI){
        manageIATurn();
    }
    return;
}

void nextTurn(){
    // We go through the entities in order, if we reach the end we go back to the start
    // Until we reach turn gauge 100 on any entity.
    int entID = game.turnManager.entityTurnID+1;
    while (true){
        
        // Go back to the start if we reached the end of the entity list.
        if (entID >= game.numEntities) entID = 0;
        
        entity* ent = &game.entities[entID];
        // Increase turn gauge based on speed for the current entity.
        ent->turnGauge += ent->speed;

        // If the turn gauge is 100 or more, it's this entity's turn.
        if (ent->turnGauge >= 100){
            game.turnManager.entityTurnID = entID;
            // Update turn state based on team
            if (ent->team == 1){
                game.turnManager.turnState = TURN_STATE_PLAYER;
                game.hudState = HUD_ACTION_MENU;
            }
            else {
                game.turnManager.turnState = TURN_STATE_AI;
                game.hudState = HUD_NONE;
            }
            printf("It's now entity %d's turn (type: %s, team: %d)\n", entID, ent->type, ent->team);

            // Reset turn gauge
            ent->turnGauge -= 100;
            
            // Calculate movement for the entity whose turn it is.
            bfs(entID, game.turnManager.movementNodes, &game.turnManager.numMovementNodes, true, ent->stamina);
            
            // Reset move and attack consumption
            game.turnManager.movementConsumed = false;
            game.turnManager.attackConsumed = false;

            // Update HUD
            updateButtons(BUTTON_NONE);
            return;
        }
        entID++;
    }
}

// Gets called on click outside of map

void updateHUD(ButtonType buttonType){

    int currentTeamTurn = game.entities[game.turnManager.entityTurnID].team;
    HudState currHudState = game.hudState;

    // Button none gets passed as argument from turnUpdate()
    if (buttonType == BUTTON_NONE){
        switch (currentTeamTurn){
            case 1:
                game.hudState = HUD_ACTION_MENU;
                updateButtons();
                return;
            case 2:
                game.hudState = HUD_NONE;
                return;
            default:
                printf("[BUG] Invalid team number (%d) when updating HUD\n", currentTeamTurn);
                return;
        }
    } else if (buttonType == BUTTON_TILE_CLICKED){
        
        game.hudState = HUD_ACTION_MENU;
        updateButtons();
        
        return;
    }

    // This should not happen
    if (currentTeamTurn == 2){
        game.hudState = HUD_NONE;
        printf("[BUG] AI turn, ignoring button click.\n");
        return;
    }

    // Action menu button handling

    if (
        currHudState == HUD_ACTION_MENU ||
        currHudState == HUD_ATTACK_SUBMENU || 
        currHudState == HUD_MOVEMENT_MODE
    ){
        // MOVE button selected
        if (buttonType == BUTTON_ACTION_MENU_MOVE){
            game.hudState = HUD_MOVEMENT_MODE;
            printf("Player selected move, setting HUD to movement mode.\n");
            updateButtons();
            return;
        
        // ATTACK button selected
        } else if (buttonType == BUTTON_ACTION_MENU_ATTACK){
            game.hudState = HUD_ATTACK_SUBMENU;
            printf("Player selected attack, setting HUD to attack submenu.\n");
            updateButtons();
            return;
        } else if (buttonType == BUTTON_ACTION_MENU_END_TURN){
            printf("Player selected end turn. Ending turn and passing to next entity.\n");
            nextTurn();
            return;
        }
    
    // Attack submenu button handling
    }
    if (currHudState == HUD_ATTACK_SUBMENU){
        turnManager* turn = &game.turnManager;
        switch (buttonType){
            case BUTTON_NORMAL_ATTACK:
                bfs(turn->entityTurnID, turn->attackNodes, &turn->numAttackNodes, false, game.entities[turn->entityTurnID].range);
                game.hudState = HUD_ATTACK_MODE;
                printf("Normal attack selected, hud set to attack mode, calculating attackable tiles.\n");
                updateButtons();
                return;
            case BUTTON_SKILL_1:
                // To be implemented when we have skills
                return;
            case BUTTON_SKILL_2:
                // To be implemented when we have skills
                return;
            case BUTTON_SKILL_3:
                // To be implemented when we have skills
                return;
            default:
                printf("[BUG] Invalid button type %d in attack submenu.\n", buttonType);
                return;
        }
    }
}
// Activates buttons based on hud state
void updateButtons(){
    
    // Hide all buttons if no HUD is active
    if (game.hudState == HUD_NONE){
        for (int i = 0; i < game.numButtons; i++){
            game.buttons[i].beingDisplayed = false;
        }
    
    } else if (
        game.hudState == HUD_ACTION_MENU ||
        game.hudState == HUD_MOVEMENT_MODE
    ){
        game.buttons[0].beingDisplayed = true;
        game.buttons[1].beingDisplayed = true;
        game.buttons[2].beingDisplayed = true;
        game.buttons[3].beingDisplayed = false;
    
    } else if (
        game.hudState == HUD_ATTACK_SUBMENU ||
        game.hudState == HUD_ATTACK_MODE
    ){
        game.buttons[0].beingDisplayed = true;
        game.buttons[1].beingDisplayed = true;
        game.buttons[2].beingDisplayed = true;
        game.buttons[3].beingDisplayed = true;
    }
    // Else
}

// This function uses breadth first search to find all reachable tiles
void bfs(int idEntity, node nodeList[MAX_NODES], int* numNodes, bool checkPathing, int maxCost){

    // Auxiliary arrays to visit neighbors
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};
    // This will help us mark visited tiles
    bool wasVisited[MAP_TILE_HEIGHT][MAP_TILE_LENGHT] = {false};
    // This will store the cost to reach said tile
    int cost[MAP_TILE_HEIGHT][MAP_TILE_LENGHT] = {0};

    int currentIndex = 0; // Counts index of the node being evaluated in the list.
    int queueEnd = 0; // Counts number of nodes validated and added to the resulting list.

    node startNode = {game.entities[idEntity].x, game.entities[idEntity].y, 0};
    int startX = startNode.x;
    int startY = startNode.y;

    // First we store the first node
    nodeList[queueEnd] = startNode;
    wasVisited[startY][startX] = true;
    cost[startY][startX] = 0;

    while (currentIndex <= queueEnd){
        node currentNode = nodeList[currentIndex];
        currentIndex++;

        // For movement, checking stamina helps take into account pathing
        // We discard nodes that cost more than the tiles the entity can move in a turn
        if (cost[currentNode.y][currentNode.x] + 1 > maxCost){
            continue; 
        }

        // We check the 4 neighbors of the current node
        for (int i = 0; i < 4; i++){
            int newX = currentNode.x + dx[i];
            int newY = currentNode.y + dy[i];

            // We check if neighbor is within bounds, walkable and not visited.
            if (newX >= 0 && newX < MAP_TILE_LENGHT && newY >= 0 && newY < MAP_TILE_HEIGHT){
                if (!wasVisited[newY][newX] && 
                    (checkPathing ? game.tilemap[newY][newX].isWalkable : true)){
                    // If it's valid, we add it to the list and mark it as visited.
                    queueEnd++;
                    wasVisited[newY][newX] = true;
                    cost[newY][newX] = cost[currentNode.y][currentNode.x] + 1;
                    nodeList[queueEnd] = (node) {newX, newY, cost[newY][newX]};
                }
            }
        }
    }

    // Store total number of nodes
    (*numNodes) = queueEnd + 1; // +1 because queueEnd is index based   
}


void moveEntityToTile(int entityId, int targetTileX, int targetTileY){
    
    entity* ent = &game.entities[entityId];
    tile* originTile = &game.tilemap[ent->y][ent->x];
    tile* targetTile = &game.tilemap[targetTileY][targetTileX];

    game.turnManager.movementConsumed = true; // Mark movement as consumed for the current turn

    // Update origin tile information
    originTile->userType = TILE_USER_NONE;
    originTile->idUser = -1; // No user
    originTile->isWalkable = true;
    originTile->userSpriteIndex = -1;

    // Update entity position
    game.entities[entityId].x = targetTileX;
    game.entities[entityId].y = targetTileY;
    
    // Update tile information
    targetTile->userType = TILE_USER_ENTITY;
    targetTile->idUser = ent->id;
    targetTile->isWalkable = false;
    targetTile->userSpriteIndex = ent->skinIndex;

    updateHUD(BUTTON_TILE_CLICKED); // Update HUD to reflect movement and return to action menu
}

void attackEntity(int attackerId, int targetId){
    entity* attacker = &game.entities[attackerId];
    entity* target = &game.entities[targetId];
    (target->HP) -= (attacker->atk);
    game.turnManager.attackConsumed = true; // Mark attack as consumed for the current turn

    updateHUD(BUTTON_TILE_CLICKED); // Update HUD to reflect attack and return to action menu
}

int lookForClosestTarget(){
    // This function will look for the closest enemy and set the attack nodes to be able to attack it.
    int entityId = game.turnManager.entityTurnID;
    entity* ent = &game.entities[entityId];

    int closestEnemyId = -1;
    int minDistance = 999;

    for (int i = 0; i < game.numEntities; i++){
        entity* potentialTarget = &game.entities[i];
        if (potentialTarget->team != ent->team){
            int distance = abs(ent->x - potentialTarget->x) + abs(ent->y - potentialTarget->y);
            if (distance < minDistance){
                minDistance = distance;
                closestEnemyId = i;
            }
        }
    } 
    bfs(entityId, game.turnManager.attackNodes, &game.turnManager.numAttackNodes, false, ent->range);
    return closestEnemyId;
}



// IA update

void manageIATurn(){
    int entityId = game.turnManager.entityTurnID;
    entity* ent = &game.entities[entityId];
    
    // Look for target
    int targetId = lookForClosestTarget();
    entity* target = &game.entities[targetId];

    // Generate attack nodes for the current entity
    bfs(entityId, game.turnManager.attackNodes, &game.turnManager.numAttackNodes, false, ent->range);

    //Tries to attack if in range
    for (int i = 0; i < game.turnManager.numAttackNodes; i++){
        node* attackNode = &game.turnManager.attackNodes[i];
        if (attackNode->x == target->x && attackNode->y == target->y){
            attackEntity(entityId, targetId);
        }
    }

    // If not in range, move towards the target
     if (!game.turnManager.attackConsumed){
        node* bestMove = NULL;
        int minDistanceToTarget = 999;

        for (int i = 0; i < game.turnManager.numMovementNodes; i++){
            node* moveNode = &game.turnManager.movementNodes[i];
            int distanceToTarget = abs(moveNode->x - target->x) + abs(moveNode->y - target->y);
            if (distanceToTarget < minDistanceToTarget){
                minDistanceToTarget = distanceToTarget;
                bestMove = moveNode;
            }
        }

        if (bestMove != NULL){
            moveEntityToTile(entityId, bestMove->x, bestMove->y);
        }
    }
      
    nextTurn();
}

