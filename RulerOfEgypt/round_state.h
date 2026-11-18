//
// Created by gonch2 on 13.10.2026.
//

#ifndef RULEROFEGYPT_ROUND_STATE_H
#define RULEROFEGYPT_ROUND_STATE_H

#include <vector>

#include "settings.h"


enum class ValidationError {
    kNotEnoughLandToSell,
    kNotEnoughWheatToSell,

    kNegativeValue,
    kNotEnoughWheatToEat,

    kNotEnoughPeopleForFarming,
    kNotEnoughLandToFarming,
    kNotEnoughWheatToFarm
};

enum class GameOverReason {
    kNone,
    kFamineRiot,
    kExtinction
};

struct RoundState {
    int population;
    int land;
    int wheat; // multiplied by 10

    int current_round;
    double hunger_percent;
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

GameOverReason UpdateState(RoundState& rs, const UpdateInfo& ui, RoundReport& r);
std::vector<ValidationError> ValidateUpdateInfo(const UpdateInfo& ui, const RoundState &rs);
bool IsValidSavedState(const RoundState& rs, const RoundReport& report);

RoundState CreateInitialState();


int GetRandom(int min, int max);
double GetRandom(double min, double max);

#endif //RULEROFEGYPT_ROUND_STATE_H
