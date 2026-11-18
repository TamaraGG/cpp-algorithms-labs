//
// Created by gonch2 on 13.10.2026.
//
#include "round_state.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "settings.h"
#include <random>


std::vector<ValidationError> ValidateUpdateInfo(const UpdateInfo &ui,
                                                  const RoundState &rs) {
    std::vector<ValidationError> errors;

    if (ui.wheat_to_eat < 0 or ui.land_to_farm < 0) {
        errors.push_back(ValidationError::kNegativeValue);
        return errors;
    }

    const std::int64_t trade = ui.land_to_buy_sell;
    const std::int64_t cost = trade * rs.acre_price;
    const std::int64_t land_after_trade = rs.land + trade;
    const std::int64_t wheat_after_trade = rs.wheat - cost;

    if (cost > rs.wheat) {
        errors.push_back(ValidationError::kNotEnoughWheatToSell);
    } else if (land_after_trade < 0) {
        errors.push_back(ValidationError::kNotEnoughLandToSell);
    } else {
        if (ui.wheat_to_eat > wheat_after_trade) {
            errors.push_back(ValidationError::kNotEnoughWheatToEat);
        }

        const std::int64_t wheat_after_food =
        std::max<std::int64_t>(0, wheat_after_trade - ui.wheat_to_eat);
        const std::int64_t farm_capacity =
            static_cast<std::int64_t>(rs.population) * kLandToFarmPerPerson;
        const std::int64_t seed_cost =
            static_cast<std::int64_t>(ui.land_to_farm) * kWheatPerAcreToSow;

        if (ui.land_to_farm > land_after_trade) {
            errors.push_back(ValidationError::kNotEnoughLandToFarming);
        } else if (ui.land_to_farm > farm_capacity) {
            errors.push_back(ValidationError::kNotEnoughPeopleForFarming);
        } else if (seed_cost > wheat_after_food) {
            errors.push_back(ValidationError::kNotEnoughWheatToFarm);
        }
    }
    return errors;
}

bool IsValidSavedState(const RoundState& rs, const RoundReport& report) {
    if (rs.current_round < 0 || rs.current_round >= kRoundNumber ||
        rs.population <= 0 || rs.land < 0 || rs.wheat < 0) {
        return false;
    }

    if (rs.acre_price < kWheatPerAcreToBuyLower ||
        rs.acre_price > kWheatPerAcreToBuyUpper ||
        !std::isfinite(rs.hunger_percent) || rs.hunger_percent < 0.0 ||
        rs.hunger_percent > kFamineMinPercentToLose) {
        return false;
    }

    if (rs.current_round == 0) {
        return rs.population == kStartPopulation && rs.land == kStartLand &&
            rs.wheat == kStartWheat && rs.hunger_percent == 0.0 &&
            report.died_from_hunger_population == 0 && report.new_population == 0 &&
            !report.is_plague && report.collected_wheat == 0 &&
            report.wheat_from_acre == 0 && report.wheat_eaten_by_rats == 0;
    }

    if (report.died_from_hunger_population < 0 ||
        report.new_population < kNewPopulationLower ||
        report.new_population > kNewPopulationUpper ||
        report.collected_wheat < 0 || report.wheat_eaten_by_rats < 0 ||
        report.wheat_from_acre < kWheatFromAcreLower ||
        report.wheat_from_acre > kWheatFromAcreUpper) {
        return false;
    }

    std::int64_t max_population = kStartPopulation;
    std::int64_t max_resources = kStartWheat +
        static_cast<std::int64_t>(kStartLand) * kWheatPerAcreToBuyUpper;
    std::int64_t max_farmed_land = 0;
    std::int64_t previous_max_population = 0;

    for (int round = 0; round < rs.current_round; ++round) {
        previous_max_population = max_population;
        max_farmed_land = max_population * kLandToFarmPerPerson;
        max_resources = max_resources * kWheatPerAcreToBuyUpper /
            kWheatPerAcreToBuyLower +
            max_farmed_land * (kWheatFromAcreUpper - kWheatPerAcreToSow);
        max_population += kNewPopulationUpper;
    }

    const std::int64_t resources = rs.wheat +
        static_cast<std::int64_t>(rs.land) * kWheatPerAcreToBuyUpper;
    if (rs.population > max_population || resources > max_resources ||
        static_cast<std::int64_t>(report.died_from_hunger_population) * 100 >
            previous_max_population * kFamineMinPercentToLose ||
        report.wheat_eaten_by_rats > static_cast<std::int64_t>(
            static_cast<double>(max_resources) * kWheatEatenByRatsCoefUpper)) {
        return false;
    }

    if (report.collected_wheat % report.wheat_from_acre != 0) {
        return false;
    }

    const int farmed_land = report.collected_wheat / report.wheat_from_acre;
    return farmed_land <= rs.land && farmed_land <= max_farmed_land;
}

RoundState CreateInitialState() {
    return RoundState(
            kStartPopulation,
            kStartLand,
            kStartWheat,
            0,
            0.0,
            GetRandom(kWheatPerAcreToBuyLower, kWheatPerAcreToBuyUpper)
        );
}

GameOverReason UpdateState(RoundState &rs, const UpdateInfo &ui, RoundReport &r) {
    // random values

    r.wheat_from_acre = GetRandom(kWheatFromAcreLower, kWheatFromAcreUpper);
    double wheat_eaten_by_rats_coef  = GetRandom(kWheatEatenByRatsCoefLower, kWheatEatenByRatsCoefUpper);

    r.is_plague = GetRandom(0.0, 1.0) < kPlagueProbability;

    // 1. buy/sell land
    const std::int64_t trade = ui.land_to_buy_sell;
    const std::int64_t cost = trade * rs.acre_price;
    const std::int64_t land_after_trade = rs.land + trade;
    const std::int64_t wheat_after_trade = rs.wheat - cost;

    rs.land = static_cast<int>(land_after_trade);
    rs.wheat = static_cast<int>(wheat_after_trade);

    // 2. farming wheat
    r.collected_wheat = ui.land_to_farm * r.wheat_from_acre; // collect
    rs.wheat += ui.land_to_farm * (r.wheat_from_acre - kWheatPerAcreToSow);

    // 3. rats
    r.wheat_eaten_by_rats = static_cast<int>(rs.wheat * wheat_eaten_by_rats_coef);
    rs.wheat -= r.wheat_eaten_by_rats;

    // 4. hunger
    int wheat_to_eat = std::min(ui.wheat_to_eat, rs.wheat);
    rs.wheat -= wheat_to_eat;
    r.died_from_hunger_population = std::max(0, rs.population - wheat_to_eat / kWheatPerPerson);

    double current_year_hunger = 0.0;
    if (rs.population > 0) {
        current_year_hunger = r.died_from_hunger_population * 100.0 / rs.population;
    }
    int completed_rounds = rs.current_round + 1;
    rs.hunger_percent = (rs.hunger_percent * rs.current_round + current_year_hunger) / completed_rounds;

    if (current_year_hunger > kFamineMinPercentToLose) {
        return GameOverReason::kFamineRiot;
    }

    // 5. new population
    double new_population = std::floor(
        r.died_from_hunger_population / 2.0 + ((50.0 - r.wheat_from_acre) * rs.wheat) / 60000.0 + 1.0
        );
    r.new_population = static_cast<int>(std::clamp(
        new_population,
        static_cast<double>(kNewPopulationLower),
        static_cast<double>(kNewPopulationUpper)));
    rs.population = rs.population - r.died_from_hunger_population + r.new_population;

    // 6. plague
    rs.population = static_cast<int>(std::floor(
        rs.population * (r.is_plague ? kPlaguePopulationDecline : 1.0)));

    if (rs.population <= 0) {
            return GameOverReason::kExtinction;
    }

    rs.acre_price = GetRandom(kWheatPerAcreToBuyLower, kWheatPerAcreToBuyUpper);

    return GameOverReason::kNone;
}

double GetRandom(const double min, const double max) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(min, max);
    return dist(gen);
}

int GetRandom(const int min, const int max) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}
