#include "game.h"

int Game_TickRound(game_t* game)
{
    switch (game->tick_type)
    {
    case TICK_PLAYERWISE:
        tick_playerwise_data_t* data = &(game->tick_information.playerwise);
        return Tick_Playerwise_Round(data, game);
    default:
        return ENXIO;
    }
}

int Game_TickStep(game_t* game)
{
    switch (game->tick_type)
    {
    case TICK_PLAYERWISE:
        tick_playerwise_data_t* data = &(game->tick_information.playerwise);
        return Tick_Playerwise_Step(data, game);
    default:
        return ENXIO;
    }
}

int Game_TickSingle(game_t* game)
{
    switch (game->tick_type)
    {
    case TICK_PLAYERWISE:
        tick_playerwise_data_t* data = &(game->tick_information.playerwise);
        return Tick_Playerwise_Single(data, game);
    default:
        return ENXIO;
    }
}