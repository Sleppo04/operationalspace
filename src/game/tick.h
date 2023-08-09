#ifndef TICK_H
#define TICK_H

typedef enum TickType {
	TICK_PLAYERWISE
} tick_type_t;

typedef struct TickPlayerWiseData {
	uint16_t current_player_index;
} tick_playerwise_data_t;

typedef union TickData {
	tick_playerwise_data_t playerwise;
} tick_data_t;

int Tick_PerformByType(enum TickType type, union TickData data, game_t* game);

#endif //TICK_H
