#include "game.h"

int Game_TickShip(game_t* game, player_t* player, gameobject_t* ship)
{
    if (player == NULL) {
        return EINVAL;
    }
    if (ship == NULL) {
        return EINVAL;
    }
    if (game == NULL) {
        return EINVAL;
    }


    // TODO: Implement this function
    //script_t* ship_tick_script = &(player->scripts.ship_tick_script);
    //action_t action = NULL;
    //int script_code = Script_GetShipAction(ship_tick_script, &action);

    return EXIT_SUCCESS;
}

int Game_TickRound(game_t* game)
{
    if (game == NULL) {
        return EINVAL;
    }

    arraylist_t*  player_ship_list;
    gameobject_t* current_ship;
    int tick_code;
    uint16_t* current_player_index = &(game->tick_current_player);
    uint32_t* current_ship_index   = &(game->tick_current_ship);


    for (uint16_t player_index = current_player_index[0]; player_index < game->player_count; player_index++) {
        player_ship_list = &((game->players + player_index)->ship_list);
        for (uint32_t ship_index = current_ship_index[0]; ship_index < player_ship_list->size; ship_index++) {
            current_ship = player_ship_list->array[ship_index];

            tick_code = Game_TickShip(game, game->players + player_index, current_ship);
            if (tick_code) {
                current_ship_index[0]   = ship_index;
                current_player_index[0] = player_index;
                return tick_code;
            }
        }
        current_ship_index[0] = 0;
    }

    current_player_index[0] = 0;

    return EXIT_SUCCESS;
}

int Game_TickStep(game_t* game)
{
    if (game == NULL) {
        return EINVAL;
    }


    uint16_t* current_player_index = &(game->tick_current_player);
    uint32_t* current_ship_index   = &(game->tick_current_ship);
    player_t*     player            = game->players + current_player_index[0];
    arraylist_t*  current_ship_list = &(player->ship_list); 
    gameobject_t* ship;
    int tick_code;

    for (size_t i = 0; i < current_ship_list->size; i++) {
        ship = current_ship_list->array[i];
        tick_code = Game_TickShip(game, player, ship);
        if (tick_code) {
            current_ship_index[0] = i;
            return tick_code;
        }
    }

    
    current_ship_index[0]   = 0;
    current_player_index[0] = (current_player_index[0] + 1) % game->player_count;

    return EXIT_SUCCESS;
}

int Game_TickSingle(game_t* game)
{
    if (game == NULL) {
        return EINVAL;
    }


    uint16_t* current_player_index   = &(game->tick_current_player);
    uint32_t* current_ship_index     = &(game->tick_current_ship);
    player_t* current_player         = game->players + current_player_index[0];
    arraylist_t* current_player_list = &(current_player->ship_list); 

    if (current_ship_index[0] > current_player_list->size) {
        current_player_index[0] = (current_player_index[0] + 1) % game->player_count;
        current_ship_index[0]   = 0;
        return EXIT_SUCCESS;
    }


    gameobject_t* current_ship = current_player_list->array[current_ship_index[0]];
    current_player             = game->players + current_player_index[0];

    int tick_code = Game_TickShip(game, current_player, current_ship);

    current_ship_index[0]++;
    return tick_code;
}
