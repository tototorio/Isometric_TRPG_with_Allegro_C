Tactical RPG (Allegro 5, C)
==========================

Turn-based tactical RPG on an isometric grid, built in C with Allegro 5. The engine is data-driven: maps, entities, objects, and textures are loaded from text files so you can add content without changing code.

Features
--------
- Isometric tilemap rendering with Allegro transforms
- Speed-based turn gauge with BFS movement and attack ranges
- Basic AI that moves toward the closest target and attacks in range
- HUD action menu and entity info cards
- Data-driven content using text config files

Requirements
------------
- GCC or Clang with C11 support
- Allegro 5 core plus image, primitives, font, and ttf addons
- make
- Linux (tested)

Debian/Ubuntu example:
sudo apt-get install liballegro5-dev

Build and Run
-------------
1) Build: make
2) Run the binary from the project root: ./game
3) Clean: make clean

Note: the default build enables AddressSanitizer and UBSan. Adjust CFLAGS in makefile if you want a release build.

Controls
--------
- Hover the mouse to show the tile marker.
- Click a tile to select it and show the entity card.
- Click MOVERSE to show movement range, then click a destination tile.
- Click ATACAR then ATK NORMAL to show attack range, then click an enemy.
- Click PASAR to end the turn.

Project Structure
-----------------
- Entry point: [main.c](main.c) calls init() and gameLoop().
- Setup and asset loading: [engine/src/setup.c](engine/src/setup.c).
- Rendering: [engine/src/renderer.c](engine/src/renderer.c).
- Input handling: [engine/src/input.c](engine/src/input.c).
- Turn system and AI: [engine/src/update.c](engine/src/update.c).

Data Files (map1)
-----------------
- Tile textures and 20x20 grid: [assets/maps/map1/tileTextures.txt](assets/maps/map1/tileTextures.txt).
- Object sprite codes: [assets/maps/map1/objects.txt](assets/maps/map1/objects.txt).
- Map layout tokens: [assets/maps/map1/mapDisposition.txt](assets/maps/map1/mapDisposition.txt).
- Entity stats and skin binding: [assets/maps/map1/entityStats.txt](assets/maps/map1/entityStats.txt).
- Skin definitions (frame counts): [assets/maps/map1/skins.txt](assets/maps/map1/skins.txt).

Token legend used in assets/maps/map1/mapDisposition.txt:
..  empty tile
XX  wall or blocked tile
Y*  enemy entity type (team 2)
Z*  ally entity type (team 1)
I*  interactive object
A* / B* / D*  static object codes

Assets
------
- HUD menu: [assets/hud/menu.png](assets/hud/menu.png).
- UI font: [assets/fonts/Minecraft.ttf](assets/fonts/Minecraft.ttf).
- Example skin frames: [assets/skins/adventurer/SW1.png](assets/skins/adventurer/SW1.png), [assets/skins/soldier/SW1.png](assets/skins/soldier/SW1.png).
- Example tile texture: [assets/tileTextures/grass.png](assets/tileTextures/grass.png).
- Example object sprite: [assets/objects/small_house.png](assets/objects/small_house.png).

Configuration
-------------
Default screen size is 1920x1080 and the map is 20x20 tiles; adjust values in [engine/include/constants.h](engine/include/constants.h).

Notes and Limitations
---------------------
- The current build loads map1 only; change the map name in [engine/src/setup.c](engine/src/setup.c) to switch.
- Skills are scaffolded in data structures but only normal attacks are active in [engine/src/update.c](engine/src/update.c).
- Interactive object stats file [assets/maps/map1/interactiveObjectsStats.txt](assets/maps/map1/interactiveObjectsStats.txt) is present but not loaded yet.
- Legacy single-file prototype lives in [proyecto.c](proyecto.c) and is not built by the default make target.
