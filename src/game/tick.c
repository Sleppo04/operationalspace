#include "tick.h"

int Tick_Playerwise_TickShip(game_t* game, player_t* player, ship_t* ship)
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

int Tick_Round(uint16_t* current_player_index, uint32_t* current_ship_index, game_t *game)
{
    if (current_player_index == NULL || current_ship_index == NULL) {
        return EINVAL;
    }
    if (game == NULL) {
        return EINVAL;
    }

    arraylist_t*  player_ship_list;
    ship_t* current_ship;
    int tick_code;
    for (uint16_t player_index = current_player_index[0]; player_index < game->player_count; player_index++) {
        player_ship_list = game->ship_lists + player_index;
        for (uint32_t ship_index = current_ship_index[0]; ship_index < player_ship_list->size; ship_index++) {
            current_ship = player_ship_list->array[ship_index];

            tick_code = Tick_Playerwise_TickShip(game, game->players + player_index, current_ship);
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

int Tick_Step(uint16_t* current_player_index, uint32_t* current_ship_index, game_t* game)
{
    if (current_player_index == NULL || current_ship_index == NULL) {
        return EINVAL;
    }
    if (game == NULL) {
        return EINVAL;
    }

    arraylist_t*  current_ship_list = game->ship_lists + current_player_index[0];
    player_t*     player            = game->players + current_player_index[0];
    ship_t* ship;
    int tick_code;
    for (size_t i = 0; i < current_ship_list->size; i++) {
        ship = current_ship_list->array[i];
        tick_code = Tick_Playerwise_TickShip(game, player, ship);
        if (tick_code) {
            current_ship_index[0] = i;
            return tick_code;
        }
    }

    
    current_ship_index[0]   = 0;
    current_player_index[0] = (current_player_index[0] + 1) % game->player_count;

    return EXIT_SUCCESS;
}

int Tick_Single(uint16_t* current_player_index, uint32_t* current_ship_index, game_t* game)
{
    if (current_player_index == NULL || current_ship_index == NULL) {
        return EINVAL;
    }
    if (game == NULL) {
        return EINVAL;
    }

    arraylist_t* current_player_list = game->ship_lists + current_player_index[0];

    if (current_ship_index[0] > current_player_list->size) {
        current_player_index[0] = (current_player_index[0] + 1) % game->player_count;
        current_ship_index[0]   = 0;
        return EXIT_SUCCESS;
    }


    ship_t*   current_ship      = current_player_list->array[current_ship_index[0]];
    player_t* current_player    = game->players + current_player_index[0];

    int tick_code = Tick_Playerwise_TickShip(game, current_player, current_ship);

    current_ship_index[0]++;
    return tick_code;
}