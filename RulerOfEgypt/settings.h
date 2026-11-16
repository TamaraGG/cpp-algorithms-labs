//
// Created by gonch2 on 13.10.2026.
//

#ifndef RULEROFEGYPT_SETTINGS_H
#define RULEROFEGYPT_SETTINGS_H
#include <string>

inline constexpr char const * FILE_NAME = "saved_game.txt";

inline constexpr int ROUNDS_NUMBER = 10;

inline constexpr int FAMINE_MIN_PERCENT_TO_LOSE = 45;

// start values
inline constexpr int START_POPULATION = 100;
inline constexpr int START_WHEAT = 28000; // multiplied by 10
inline constexpr int START_LAND = 1000;

// loose rules
inline constexpr int POPULATION_TO_LOOSE = 0;
inline constexpr float POPULATION_DIFFERENCE_TO_LOOSE = 0.45f;

// coefficients
// 1. population
inline constexpr float PLAGUE_PROBABILITY = 0.15f;
inline constexpr float PLAGUE_POPULATION_DECLINE = 0.5f;

inline constexpr int NEW_POPULATION_LOWER = 0;
inline constexpr int NEW_POPULATION_UPPER = 50;

inline constexpr int WHEAT_PER_PERSON = 200; // multiplied by 10

// 2. acres of land
inline constexpr int WHEAT_PER_ACRE_TO_BUY_LOWER = 170; // multiplied by 10
inline constexpr int WHEAT_PER_ACRE_TO_BUY_UPPER = 260; // multiplied by 10
inline constexpr int WHEAT_PER_ACRE_TO_SOW = 5;         // multiplied by 10

// 3. wheat bushels
inline constexpr int WHEAT_FROM_ACRE_LOWER = 10; // multiplied by 10
inline constexpr int WHEAT_FROM_ACRE_UPPER = 60; // multiplied by 10

inline constexpr float WHEAT_EATEN_BY_RATS_COEF_LOWER = 0.f;
inline constexpr float WHEAT_EATEN_BY_RATS_COEF_UPPER = 0.07f;

// 4. farming
inline constexpr int LAND_TO_FARM_PER_PERSON = 10;

#endif //RULEROFEGYPT_SETTINGS_H
