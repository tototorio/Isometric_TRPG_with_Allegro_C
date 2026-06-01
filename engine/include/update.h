#ifndef UPDATE_H
#define UPDATE_H

#include "commons.h"

void updateGame();
void nextTurn();
void bfs(int idEntity, node nodeList[MAX_NODES], int* numNodes, bool checkStamina, int maxCost);

void moveEntityToTile(int entityId, int targetTileX, int targetTileY);
void attackEntity(int attackerId, int targetId);  
void updateButtons();
void updateHUD(ButtonType buttonType);
void manageIATurn();
int lookForClosestTarget();


#endif