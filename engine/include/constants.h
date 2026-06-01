#ifndef CONSTANTS_H
#define CONSTANTS_H

//Dimensiones
#define SCREEN_PIXEL_HEIGHT 1080 //Screen height in pixels
#define SCREEN_PIXEL_WIDTH 1920 //Screen width in pixels
#define TILE_PIXEL_HEIGHT 48 //Tile height in pixels
#define TILE_PIXEL_LENGHT 48 //Tile width in pixels
#define MAP_TILE_HEIGHT 20 //Tilemap height in tiles
#define MAP_TILE_LENGHT 20 //Tilemap width in tiles
#define MAP_PIXEL_HEIGHT MAP_TILE_HEIGHT * TILE_PIXEL_HEIGHT
#define MAP_PIXEL_LENGHT MAP_TILE_LENGHT * TILE_PIXEL_LENGHT

//Limites de arreglos
#define MAX_ENTITIES 20
#define MAX_OBJECTS 150
#define MAX_SPELLS 3
#define MAX_FRAMES 20
#define MAX_ANIMATIONS 10
#define MAX_IDLE_FRAMES 4
#define MAX_SPRITES 50
#define MAX_TEXTURES 50
#define MAX_SKINS 20
#define MAX_FONTS 10
#define MAX_UID_ASSETS 20
#define MAX_BUTTONS 20
#define MAX_NODES MAP_TILE_LENGHT*MAP_TILE_HEIGHT
//Estados para manejar turnos
#define ESTADO_ESPERA 0
#define ESTADO_JUGADOR 1
#define ESTADO_IA 2
#define ESTADO_DIBUJANDO 3
//Estados para pantalla final
#define JUGANDO 0
#define FIN 1

#endif