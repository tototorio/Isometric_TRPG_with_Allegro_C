#include "../include/game.h"

void gameLoop(){

    ALLEGRO_EVENT currentEvent;

    while(true){
        //We use a wait function to avoid busy waiting.
        al_wait_for_event(game.eventQueue, &currentEvent);
        while (true){
            handleEvent(currentEvent);
            //If no event in queue, we go back to waiting.
            if (!al_get_next_event(game.eventQueue, &currentEvent)){
                break;
            }
        }
    }
}

//Helper function to centralize event handling logic.
void handleEvent(ALLEGRO_EVENT event){
    if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
        exit(0);
    }
    else if (event.type == ALLEGRO_EVENT_TIMER){
        updateGame();
        renderFrame();
    }
    else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
        handleClick(event.mouse.x, event.mouse.y);
    }
    else if (event.type == ALLEGRO_EVENT_MOUSE_AXES){
        handleMouseMove(event.mouse.x, event.mouse.y);
    }
    else if (event.type == ALLEGRO_EVENT_KEY_DOWN && game.gameState == GAME_STATE_END_SCREEN){
        //Handle key down event.
    }
    else if (event.type == ALLEGRO_EVENT_KEY_UP && game.gameState == GAME_STATE_END_SCREEN){
        //Handle key up event.
    }   
}

