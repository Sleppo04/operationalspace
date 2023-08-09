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

    

    return EXIT_SUCCESS;
}

int Tick_Playerwise_Step(tick_playerwise_data_t *data, game_t *game)
{
    return 0;
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

    while (data->current_ship_index > current_player_list->size) {
        data->current_player_index = (data->current_player_index + 1) % game->player_count;
        data->current_ship_index   = 0;
        current_player_list = game->ship_lists + data->current_player_index;
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