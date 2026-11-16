//
// Created by gonch2 on 13.10.2026.
//

#ifndef RULEROFEGYPT_ROUND_STATE_H
#define RULEROFEGYPT_ROUND_STATE_H

#include <vector>

#include "settings.h"


enum class ValidationError {
    NOT_ENOUGH_LAND_TO_SELL,
    NOT_ENOUGH_WHEAT_TO_SELL,

    NEGATIVE_VALUE,
    NOT_ENOUGH_WHEAT_TO_EAT,

    NOT_ENOUGH_PEOPLE_FOR_FARM,
    NOT_ENOUGH_LAND_TO_FARM,
    NOT_ENOUGH_WHEAT_TO_FARM
};

enum class GameOverReason {
    NONE,
    FAMINE_RIOT,
    EXTINCTION
};

struct RoundState {
    int population;
    int land;
    int wheat; // multiplied by 10

    int current_round;
    float hunger_percent;
    int acre_price;  // multiplied by 10
};


struct RoundReport {
    int died_from_hunger_population;
    int new_population;
    bool is_plague;
    int collected_wheat;
    int wheat_from_acre;      // multiplied by 10
    int wheat_eaten_by_rats;  // multiplied by 10
};

struct UpdateInfo {
    int land_to_buy_sell;
    int wheat_to_eat;  // multiplied by 10
    int land_to_farm;
};

GameOverReason update_state(RoundState& rs, const UpdateInfo& ui, RoundReport& r);
std::vector<ValidationError> validate_update_info(const UpdateInfo& ui, const RoundState &rs);
bool is_valid_saved_state(const RoundState& rs, const RoundReport& report);

RoundState create_initial_state();


int get_random(int min, int max);
float get_random(float min, float max);

#endif //RULEROFEGYPT_ROUND_STATE_H
