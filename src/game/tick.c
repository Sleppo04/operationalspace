#include "tick.h"

int Tick_Playerwise_TickShip(game_t* game, player_t* player, gameobject_t* ship)
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
    //action_t* actions = NULL;
    //int script_code = Script_ComputeShipActions(ship_tick_script, &actions);

    return EXIT_SUCCESS;
}

int Tick_Playerwise_Round(tick_playerwise_data_t *data, game_t *game)
{
    if (data == NULL) {
        return EINVAL;
    }
    if (game == NULL) {
        return EINVAL;
    }

    arraylist_t*  player_ship_list;
    gameobject_t* current_ship;
    int tick_code;
    for (uint16_t player_index = data->current_player_index; player_index < game->player_count; player_index++) {
        player_ship_list = game->ship_lists + player_index;
        for (size_t ship_index = data->current_ship_index; ship_index < player_ship_list->size; ship_index++) {
            current_ship = player_ship_list->array[ship_index];

            tick_code = Tick_Playerwise_TickShip(game, game->players + player_index, current_ship);
            if (tick_code) {
                data->current_ship_index   = ship_index;
                data->current_player_index = player_index;
                return tick_code;
            }
        }
        data->current_ship_index = 0;
    }

    data->current_player_index = 0;

    return EXIT_SUCCESS;
}

int Tick_Playerwise_Step(tick_playerwise_data_t *data, game_t *game)
{
    if (data == NULL) {
        return EINVAL;
    }
    if (game == NULL) {
        return EINVAL;
    }

    arraylist_t*  current_ship_list = game->ship_lists + data->current_player_index;
    player_t*     player            = game->players + data->current_player_index;
    gameobject_t* ship;
    int tick_code;
    for (size_t i = 0; i < current_ship_list->size; i++) {
        ship = current_ship_list->array[i];
        tick_code = Tick_Playerwise_TickShip(game, player, ship);
        if (tick_code) {
            data->current_ship_index = i;
            return tick_code;
        }
    }

    
    data->current_ship_index   = 0;
    data->current_player_index = (data->current_player_index + 1) % game->player_count;

    return EXIT_SUCCESS;
}

int Tick_Playerwise_Single(tick_playerwise_data_t *data, game_t *game)
{
    if (data == NULL) {
        return EINVAL;
    }
    if (game == NULL) {
        return EINVAL;
    }

    arraylist_t* current_player_list = game->ship_lists + data->current_player_index;

    if (data->current_ship_index > current_player_list->size) {
        data->current_player_index = (data->current_player_index + 1) % game->player_count;
        data->current_ship_index   = 0;
        return EXIT_SUCCESS;
    }


    gameobject_t* current_ship   = current_player_list->array[data->current_ship_index];
    player_t*     current_player = game->players + data->current_player_index;

    int tick_code = Tick_Playerwise_TickShip(game, current_player, current_ship);

    data->current_ship_index++;
    return tick_code;
}

int Tick_Playerwise_Init_Data(tick_playerwise_data_t *data)
{
    if (data == NULL) {
        return EINVAL;
    }

    data->current_player_index = 0;
    data->current_ship_index   = 0;

    return EXIT_SUCCESS;
}