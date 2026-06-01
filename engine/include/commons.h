//Base inclusions for multiple modules.

#ifndef COMMONS_H
#define COMMONS_H

//Allegro
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

//Standard C
#include <stdio.h>  
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

//Constants and structures
#include "constants.h"
#include "structures.h"

//Export game context to be used across modules 
extern gameContext game;

#endif