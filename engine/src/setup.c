// This module sets up the game. It initializes the game state, sets up the game board, and prepares any necessary resources for the game to run.
#include "../include/setup.h"

void init(){
    
    initializeAllegro();

    initializeGameVariables();

    createMatrices();

    createTilemap();
    
    loadMapAssets("map1");

    createHUD();

    game.assetManager.hudAssets[0] = al_load_bitmap("assets/hud/menu.png");
    game.assetManager.numHUDAssets = 1;

    game.assetManager.fonts[0] = al_load_ttf_font("assets/fonts/Minecraft.ttf", 20, 0);
    game.assetManager.numFonts = 1;

    // Initialize turn cycle
    game.turnManager.entityTurnID = -1; // The first call of nextTurn() will set it to 0.
    nextTurn(); // We initialize the first turn to set the turn manager variables correctly.

    startQueue();
}

void initializeAllegro(){

    // Initialize Allegro and its addons
    if (!al_init()){
        printf("Error when initializing Allegro\n"); return;
    }
    if (!al_install_mouse()){
        printf("Error when initializing Mouse\n"); return;
    }
    if (!al_init_image_addon()){
        printf("Error when initializing Image addon\n"); return;
    }
    if (!al_init_primitives_addon()){
        printf("Error when initializing Primitives addon\n"); return;
    }
    if (!al_install_keyboard()){
        printf("Error when initializing Keyboard\n"); return;
    }
    if (!al_init_font_addon()){
        printf("Error when initializing Font addon\n"); return;
    }
    if (!al_init_ttf_addon()){
        printf("Error when initializing TTF addon\n"); return;
    }
}

void initializeGameVariables(){

    game.bitmap = al_create_bitmap(MAP_PIXEL_LENGHT, MAP_PIXEL_HEIGHT);
    if (game.bitmap == NULL) {printf("Error when initializing main bitmap\n"); return;}
    
    game.frameCounter = al_create_timer(1.0/30.0);
    if (game.frameCounter == NULL) {printf("Error when initializing frame counter\n"); return;}
    
    game.eventQueue = al_create_event_queue();
    if (game.eventQueue == NULL) {printf("Error when initializing event queue\n"); return;}
    
    // Display settings
    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    al_set_new_display_option(ALLEGRO_COLOR_SIZE, 32, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);

    game.display = al_create_display(SCREEN_PIXEL_WIDTH, SCREEN_PIXEL_HEIGHT);
    if (game.display == NULL) {printf("Error when initializing display\n"); return;} 

    // Window name
    al_set_window_title(game.display, "Tactical RPG with Allegro");

}

void createMatrices(){

    //// Isometric matrix creation
    // Get center of the bitmap in pixels and define angle of rotation
    float mapPixelCenterX = MAP_PIXEL_LENGHT/2.0;
    float mapPixelCenterY = MAP_PIXEL_HEIGHT/2.0;
    float angle = ALLEGRO_PI/4;
    //1. Identity matrix for starters
    al_identity_transform(&game.assetManager.toIsometricMatrix); 
    //2. Before rotating, we have to move it to the center of the object.
    // This is because the default rotation is made off the right left corner of any bitmap.
    al_translate_transform(&game.assetManager.toIsometricMatrix, -(mapPixelCenterX), -(mapPixelCenterY)); 
    //3. Now we rotate it 45 degrees.
    al_rotate_transform(&game.assetManager.toIsometricMatrix, angle);
    //4. Then we shrink it vertically to perfect the isometric view.
    al_scale_transform(&game.assetManager.toIsometricMatrix, 1.0, 0.5);         
    //5. Lastly, we move it to the center of the screen, so the object is centered when drawn.
    al_translate_transform(&game.assetManager.toIsometricMatrix, SCREEN_PIXEL_WIDTH/2.0, SCREEN_PIXEL_HEIGHT/2.0);

    //// Reverse isometric matrix
    // Useful to get tileset coordinates when clicking on the screen.
    al_copy_transform(&game.assetManager.reverseIsometricMatrix, &game.assetManager.toIsometricMatrix);
    al_invert_transform(&game.assetManager.reverseIsometricMatrix);

    //// Identity matrix
    // Needed to reset transformations when drawing elements that don't need transformation.
    al_identity_transform(&game.assetManager.identityMatrix);
}

void createTilemap(){

    for (int j = 0; j < MAP_TILE_HEIGHT; j++){
        for (int i = 0; i < MAP_TILE_LENGHT; i++){
            
            // Get tile center in pixels.
            float pixelX = (TILE_PIXEL_LENGHT*i) + (TILE_PIXEL_LENGHT/2.0f); 
            float pixelY = (TILE_PIXEL_HEIGHT*j) + (TILE_PIXEL_HEIGHT/2.0f);
            
            // Using the isometric matrix, we convert the pixel coordinates to isometric coordinates.
            al_transform_coordinates(&game.assetManager.toIsometricMatrix, &pixelX, &pixelY);
            
            // Store information and set default values.
            game.tilemap[j][i].centerX = pixelX;
            game.tilemap[j][i].centerY = pixelY;
            game.tilemap[j][i].isWalkable = true;
            game.tilemap[j][i].idUser= -1;

        }
    }
}

void loadMapAssets(char* map){
    
    AssetDiccionaryNode objectDiccionary[MAX_SPRITES];

    char buffer[256]; //Used to concatenate file paths and strings
    snprintf(buffer, sizeof(buffer), "assets/maps/%s/tileTextures.txt", map);
    loadTileTextures(buffer);

    snprintf(buffer, sizeof(buffer), "assets/maps/%s/skins.txt", map);
    loadEntitySprites(buffer);

    snprintf(buffer, sizeof(buffer), "assets/maps/%s/objects.txt", map);
    loadObjectSprites(buffer, objectDiccionary);

    snprintf(buffer, sizeof(buffer), "assets/maps/%s/mapDisposition.txt", map);
    loadMapDisposition(buffer, objectDiccionary);

    snprintf(buffer, sizeof(buffer), "assets/maps/%s/entityStats.txt", map);
    loadEntityStats(buffer);

}

// This will create a temporary diccionary to link object and sprites
void loadEntitySprites(char* filename){

    //Auxiliary variables
    char auxName[50];
    int auxNumFrames;
    int auxIndex = 0; // Counter for the amount of different skins loaded
    char buffer[100];
    

    // Open file
    FILE* txt = fopen(filename, "r");
    if (!txt) {
        perror("Error opening skins instructions file");
        return;
    }

    while (fgets(buffer, sizeof(buffer), txt)) {
        
        // Remove newline and skip empty lines
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) == 0) continue;

        // Parse the line with sscanf
        if (sscanf(buffer, "name=%[^;];numFrames=%d", auxName, &auxNumFrames) == 2) {
            // Store skin information 
            strcpy(game.assetManager.entitySprites[auxIndex].name, auxName);
            game.assetManager.entitySprites[auxIndex].numFrames = auxNumFrames;
            // Load all frames
            loadSkinFrames(auxIndex);
            // Increase index for the next skin
            auxIndex++;
        } else {
            printf("Error parsing skin information from line: %s\n", buffer);
        }
    }
    // Store total number of skins
    game.assetManager.numEntitySprites = auxIndex; 
}

void loadSkinFrames (int index){
    
    char pathBuffer[150];

    for (int dir = 0; dir < 4; dir++){ //For each facing direction
        char* dirName;
        switch (dir) {
                case 0:
                    dirName = "SW";
                    break;
                case 1:
                    dirName = "NW";
                    break;
                case 2:
                    dirName = "SE";
                    break;    
                case 3:
                    dirName = "NE";
                    break;
                default:
                    break;
        }
        for (int j = 0; j < game.assetManager.entitySprites[index].numFrames; j++){

            snprintf(
                pathBuffer, sizeof(pathBuffer), 
                "assets/skins/%s/%s%d.png", 
                game.assetManager.entitySprites[index].name, dirName, j+1 
            );
            // Load frame and store it in the skin structure
            game.assetManager.entitySprites[index].frames[dir][j] = al_load_bitmap(pathBuffer);

        }
    }
}

void loadObjectSprites(char* filename, AssetDiccionaryNode objectDiccionary[MAX_SPRITES]){
    
    //Auxiliary variables
    char bufferPath1[100];
    char line[150]; // Line buffer for reading the file
    char auxCode[5];
    int auxIndex = 0; // Counter for the amount of different assets loaded

    // Open file
    FILE* txt = fopen(filename, "r");
    if (!txt) {
        perror("Error opening object sprites file");
        return;
    }

    while (fgets(line, sizeof(line), txt)) {
        
        // Remove newline and skip empty lines
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;
        
        // Parse the line with sscanf
        if (sscanf(line, "%4[^:]:%99s", auxCode, bufferPath1) == 2) {
            
            // Store information in diccionary
            objectDiccionary[auxIndex].index = auxIndex;
            strcpy(objectDiccionary[auxIndex].code, auxCode);
            // Load and store sprite in the game context
            game.assetManager.objectSprites[auxIndex] = al_load_bitmap(bufferPath1);
            // Increase index for the next pair
            auxIndex++;

        }
    }
    game.assetManager.numObjectSprites = auxIndex; // Store total number of sprites

}

void loadMapDisposition(char* filename, AssetDiccionaryNode objectDiccionary[MAX_SPRITES]){
    // Auxiliary variables
    char line[150]; // Line buffer for reading the file
    int x = 0, y = 0; // Counter for coordinates
    
    // Open file
    FILE* txtTilemap = fopen(filename, "r");
    if (!txtTilemap) {
        perror("Error opening map distribution file");
        return;
    }

    //We initialize values to start counting and indexing
    game.numEntities = 0;
    game.numObjects = 0;

    while (fgets(line, sizeof(line), txtTilemap) != NULL) {
        
        // Remove newline and skip empty lines
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        // Use strtok to get each code in the line
        char* token = strtok(line, " ");
        x = 0;
        while (token != NULL && x < MAP_TILE_LENGHT) {
            
            if (strcmp(token, "..") == 0){
                
                // Mark free tiles
                tile* tile = &game.tilemap[y][x];
                tile->idUser = -1; // No user
                tile->isWalkable = true;
                tile->userType = TILE_USER_NONE;

                token = strtok(NULL, " ");
                x++;
                continue; // Skip empty tiles
            }

            // First we mark the tile as non-walkeable
            game.tilemap[y][x].isWalkable = false;

            // Invisible wall case
            if (strcmp(token, "XX") == 0){
                // Basic block
                game.tilemap[y][x].userType = TILE_USER_STATIC_OBJECT;
                game.tilemap[y][x].idUser = -1; // No object linked,
                token = strtok(NULL, " ");
                x++;
                continue;
            }

            // Entity case
            if (token[0] == 'Y' || token[0] == 'Z') {        
                // Update tile information
                tile* tile = &game.tilemap[y][x];
                tile->userType = TILE_USER_ENTITY;
                tile->idUser = game.numEntities;

                // Update entity information
                entity* entity = &game.entities[game.numEntities];
                entity->id = game.numEntities;
                strcpy(entity->type, token);
                entity->x = x;
                entity->y = y;
                entity->team = (token[0] == 'Z') ? 1 : 2;
                entity->facing = 0; // Default facing direction

                // Increase entity counter
                game.numEntities++; 

                //
            } 

            // Object case
            else {
                // Interactive object case
                if (token[0] == 'I') {
                    // Update tile information
                    tile* tile = &game.tilemap[y][x];
                    tile->userType = TILE_USER_INTERACTIVE_OBJECT;
                    tile->idUser = game.numObjects;

                    // Update interactive object information
                    object* obj = &game.objects[game.numObjects];
                    obj->id = game.numObjects;
                    obj->userType = TILE_USER_INTERACTIVE_OBJECT;
                    strcpy(obj->type, token);
                    obj->x = x;
                    obj->y = y;
                    
                }

                // Static object case
                else {  
                    // Update tile information
                    tile* tile = &game.tilemap[y][x];
                    tile->userType = TILE_USER_STATIC_OBJECT;
                    tile->idUser = game.numObjects;

                    // Update object information
                    object* obj = &game.objects[game.numObjects];
                    obj->id = game.numObjects;
                    obj->userType = TILE_USER_STATIC_OBJECT;
                    strcpy(obj->type, token);
                    obj->x = x;
                    obj->y = y;

                }
                
                // Using the dicctionary, we assing object sprite index (entity gets handled later)
                for (int i = 0; i < game.assetManager.numObjectSprites; i++){
                    if (strcmp(token, objectDiccionary[i].code) == 0){
                        game.objects[game.numObjects].spriteIndex = i;
                        break;
                    }
                }
                // Increase interactive object counter
                game.numObjects++;
            }

            //NextToken
            token = strtok(NULL, " ");
            x++;
        }
        // Height check
        y++;
        if (y == MAP_TILE_HEIGHT) {
            break; // We stop once we reached the maximum height.
        }
    }
}

void loadEntityStats(char* filename) {
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening entity stats file");
        return;
    }
    
    char line[200];
    char type[5];
    char name[50];
    int atk, range, stamina, speed, maxHP, maxMP;
    char skinID[50];
    
    // Read file line by line
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;
        
        // Found a new entity type block
        if (sscanf(line, "type=%4s", type) == 1) {
            
            // Read the next 7 lines of stats
            fgets(line, sizeof(line), file); sscanf(line, "name=%49s", name);
            fgets(line, sizeof(line), file); sscanf(line, "atk=%d", &atk);
            fgets(line, sizeof(line), file); sscanf(line, "range=%d", &range);
            fgets(line, sizeof(line), file); sscanf(line, "sta=%d", &stamina);
            fgets(line, sizeof(line), file); sscanf(line, "spd=%d", &speed);
            fgets(line, sizeof(line), file); sscanf(line, "HP=%d", &maxHP);
            fgets(line, sizeof(line), file); sscanf(line, "MP=%d", &maxMP);
            fgets(line, sizeof(line), file); sscanf(line, "skinID=%49s", skinID);
            
            // Now loop through ALL entities and apply if type matches
            for (int i = 0; i < game.numEntities; i++) {
                entity* ent = &game.entities[i];
                
                if (strcmp(ent->type, type) == 0) {
                    // Apply all stats
                    strcpy(ent->name, name);
                    ent->atk = atk;
                    ent->range = range;
                    ent->stamina = stamina;
                    ent->speed = speed;
                    ent->maxHP = maxHP;
                    ent->HP = maxHP;
                    ent->maxMP = maxMP;
                    ent->MP = maxMP;
                    
                    // Find and link skin
                    for (int k = 0; k < game.assetManager.numEntitySprites; k++) {
                        if (strcmp(game.assetManager.entitySprites[k].name, skinID) == 0) {
                            ent->skinIndex = k;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    fclose(file);
}

void loadTileTextures(char* filename){
    
    // Auxiliary variables
    char auxCode[5];
    char auxPath[100];
    int auxIndex = 0; // Counter for the amount of different textures loaded
    char line[150]; // Line buffer for reading the file
    int x = 0, y = 0; // Counter for coordinates
    AssetDiccionaryNode textureDiccionary[MAX_TEXTURES]; //Temporary diccionary
    
    // Open file
    FILE* txtTilemap = fopen(filename, "r");
    if (!txtTilemap) {
        perror("Error opening tilemap instructions file");
        return;
    }
    
    // Scan until no more code:path pairs are found.
    while (fgets(line, sizeof(line), txtTilemap) != NULL) {
        
        // Remove newline and skip empty lines
        line[strcspn(line, "\n")] = '\0';
        if(strlen(line) == 0) continue;
        
        // End signal for code:path pairs
        if (strcmp(line, "stop") == 0) break; 

        // Parse the line with sscanf
        if (sscanf(line, "%4[^:]:%99s", auxCode, auxPath) == 2) {
            
            // Store information in diccionary
            textureDiccionary[auxIndex].index = auxIndex;
            strcpy(textureDiccionary[auxIndex].code, auxCode);
            // Load and store texture in the game context
            game.assetManager.tileTextures[auxIndex] = al_load_bitmap(auxPath);
            // Increase index for the next pair
            auxIndex++;

        }
    }

    game.assetManager.numTileTextures = auxIndex; // Store total number of textures

    // In the same file there will be instructions for what texture belongs to each tile.
    while (fgets(line, sizeof(line), txtTilemap) != NULL) {
        
        // Remove newline and skip empty lines
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        // Use strtok to get each code in the line
        char* token = strtok(line, " ");
        x = 0;
        while (token != NULL && x < MAP_TILE_LENGHT) {
            
            // Using the diccionary of codes, we assing textures to the corresponding tiles (using index).
            for (int i = 0; i < game.assetManager.numTileTextures; i++){
                if (strcmp(token, textureDiccionary[i].code) == 0){
                    game.tilemap[y][x].textureIndex = i;
                    break;
                }
            }

            //NextToken
            token = strtok(NULL, " ");
            x++;
        }
        // Height check
        y++;
        if (y == MAP_TILE_HEIGHT) {
            break; // We stop once we reached the maximum height.
        }
    }

}

void createHUD(){

    int actionMenuButtonsX = SCREEN_PIXEL_WIDTH*0.037; //This will decide the left horizontal padding
    int actionMenuButtonsY= SCREEN_PIXEL_HEIGHT*0.815; //This will decide the height of all tree buttons
    int actionMenuButtonsWidth = SCREEN_PIXEL_WIDTH*0.07;
    int actionMenuButtonsHeight = SCREEN_PIXEL_HEIGHT*0.029;
    int actionMenuButtonsSpacing = SCREEN_PIXEL_HEIGHT*0.011; 

    int attackSubMenuButtonsX = SCREEN_PIXEL_WIDTH*0.15; //This will decide the left horizontal padding


    createButton(
        0, BUTTON_ACTION_MENU_MOVE, "MOVERSE", 
        actionMenuButtonsX, 
        actionMenuButtonsY,
        actionMenuButtonsWidth, 
        actionMenuButtonsHeight
    );
    createButton(
        1, BUTTON_ACTION_MENU_ATTACK, "ATACAR", 
        actionMenuButtonsX, 
        actionMenuButtonsY + actionMenuButtonsHeight + actionMenuButtonsSpacing*2, 
        actionMenuButtonsWidth, 
        actionMenuButtonsHeight
    );
    createButton(
        2, BUTTON_ACTION_MENU_END_TURN, "PASAR", 
        actionMenuButtonsX, 
        actionMenuButtonsY + 2*(actionMenuButtonsHeight + actionMenuButtonsSpacing*2), 
        actionMenuButtonsWidth, 
        actionMenuButtonsHeight
    );
    createButton(
        3, BUTTON_NORMAL_ATTACK, "ATK NORMAL", 
        attackSubMenuButtonsX, 
        actionMenuButtonsY, 
        actionMenuButtonsWidth, 
        actionMenuButtonsHeight
    );

}

void createButton(
    int id, ButtonType type, char* label, 
    float x, float y, float width, float height
){
    //Register in game context
    button* btn = &game.buttons[id];
    btn->id= id;
    btn->type = type;
    strcpy(btn->label, label);
    btn->x = x;
    btn->y = y;
    btn->width = width; 
    btn->height = height;
    btn->beingDisplayed = false;

    game.numButtons++;
}

void startQueue(){

    al_register_event_source(game.eventQueue, al_get_mouse_event_source());
    al_register_event_source(game.eventQueue, al_get_display_event_source(game.display));
    al_register_event_source(game.eventQueue, al_get_timer_event_source(game.frameCounter));
    al_register_event_source(game.eventQueue, al_get_keyboard_event_source());

    al_start_timer(game.frameCounter);
}

