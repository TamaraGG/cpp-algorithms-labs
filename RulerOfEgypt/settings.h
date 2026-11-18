//
// Created by gonch2 on 13.10.2026.
//

#ifndef RULEROFEGYPT_SETTINGS_H
#define RULEROFEGYPT_SETTINGS_H
#include <string>

inline constexpr char const* kFileName = "saved_game.txt";

inline constexpr int kRoundNumber = 10;

inline constexpr int kFamineMinPercentToLose = 45;

// start values
inline constexpr int kStartPopulation = 100;
inline constexpr int kStartWheat = 28000; // multiplied by 10
inline constexpr int kStartLand = 1000;

// loose rules
inline constexpr int kPopulationToLoose = 0;
inline constexpr double kPopulationDifferenceToLoose = 0.45;

// coefficients
// 1. population
inline constexpr double kPlagueProbability = 0.15;
inline constexpr double kPlaguePopulationDecline = 0.5;

inline constexpr int kNewPopulationLower = 0;
inline constexpr int kNewPopulationUpper = 50;

inline constexpr int kWheatPerPerson = 200; // multiplied by 10

// 2. acres of land
inline constexpr int kWheatPerAcreToBuyLower = 170; // multiplied by 10
inline constexpr int kWheatPerAcreToBuyUpper = 260; // multiplied by 10
inline constexpr int kWheatPerAcreToSow = 5;         // multiplied by 10

// 3. wheat bushels
inline constexpr int kWheatFromAcreLower = 10; // multiplied by 10
inline constexpr int kWheatFromAcreUpper = 60; // multiplied by 10

inline constexpr double kWheatEatenByRatsCoefLower = 0.0;
inline constexpr double kWheatEatenByRatsCoefUpper = 0.07;

// 4. farming
inline constexpr int kLandToFarmPerPerson = 10;

#endif //RULEROFEGYPT_SETTINGS_H
