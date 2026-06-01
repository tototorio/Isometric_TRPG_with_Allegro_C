#ifndef INPUT_H
#define INPUT_H

#include "commons.h"

void handleClick(float x, float y);
void handleMouseMove(float x, float y);
void handleButtonClick(int buttonId);
void handleMovementClick(int tileX, int tileY);
void handleAttackClick(int tileX, int tileY);

#endif