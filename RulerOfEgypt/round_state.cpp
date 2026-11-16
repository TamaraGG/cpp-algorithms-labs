//
// Created by gonch2 on 13.10.2026.
//
#include "round_state.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "settings.h"
#include <random>


std::vector<ValidationError> validate_update_info(const UpdateInfo &ui,
                                                  const RoundState &rs) {
    std::vector<ValidationError> errors;

    if (ui.wheat_to_eat < 0 or ui.land_to_farm < 0) {
        errors.push_back(ValidationError::NEGATIVE_VALUE);
        return errors;
    }

    const std::int64_t trade = ui.land_to_buy_sell;
    const std::int64_t cost = trade * rs.acre_price;
    const std::int64_t land_after_trade = rs.land + trade;
    const std::int64_t wheat_after_trade = rs.wheat - cost;

    if (cost > rs.wheat) {
        errors.push_back(ValidationError::NOT_ENOUGH_WHEAT_TO_SELL);
    }
    else if (land_after_trade < 0) {
        errors.push_back(ValidationError::NOT_ENOUGH_LAND_TO_SELL);
    }
    else {
        if (ui.wheat_to_eat > wheat_after_trade) {
            errors.push_back(ValidationError::NOT_ENOUGH_WHEAT_TO_EAT);
        }

        const std::int64_t wheat_after_food =
        std::max<std::int64_t>(0, wheat_after_trade - ui.wheat_to_eat);
        const std::int64_t farm_capacity =
            static_cast<std::int64_t>(rs.population) * LAND_TO_FARM_PER_PERSON;
        const std::int64_t seed_cost =
            static_cast<std::int64_t>(ui.land_to_farm) * WHEAT_PER_ACRE_TO_SOW;

        if (ui.land_to_farm > land_after_trade) {
            errors.push_back(ValidationError::NOT_ENOUGH_LAND_TO_FARM);
        }
        else if (ui.land_to_farm > farm_capacity) {
            errors.push_back(ValidationError::NOT_ENOUGH_PEOPLE_FOR_FARM);
        }
        else if (seed_cost > wheat_after_food) {
            errors.push_back(ValidationError::NOT_ENOUGH_WHEAT_TO_FARM);
        }
    }
    return errors;
}

bool is_valid_saved_state(const RoundState& rs, const RoundReport& report) {
    if (rs.current_round < 0 || rs.current_round >= ROUNDS_NUMBER ||
        rs.population <= 0 || rs.land < 0 || rs.wheat < 0) {
        return false;
    }

    if (rs.acre_price < WHEAT_PER_ACRE_TO_BUY_LOWER ||
        rs.acre_price > WHEAT_PER_ACRE_TO_BUY_UPPER ||
        !std::isfinite(rs.hunger_percent) || rs.hunger_percent < 0.0 ||
        rs.hunger_percent > FAMINE_MIN_PERCENT_TO_LOSE) {
        return false;
    }

    if (rs.current_round == 0) {
        return rs.population == START_POPULATION && rs.land == START_LAND &&
            rs.wheat == START_WHEAT && rs.hunger_percent == 0.0 &&
            report.died_from_hunger_population == 0 && report.new_population == 0 &&
            !report.is_plague && report.collected_wheat == 0 &&
            report.wheat_from_acre == 0 && report.wheat_eaten_by_rats == 0;
    }

    if (report.died_from_hunger_population < 0 ||
        report.new_population < NEW_POPULATION_LOWER ||
        report.new_population > NEW_POPULATION_UPPER ||
        report.collected_wheat < 0 || report.wheat_eaten_by_rats < 0 ||
        report.wheat_from_acre < WHEAT_FROM_ACRE_LOWER ||
        report.wheat_from_acre > WHEAT_FROM_ACRE_UPPER) {
        return false;
    }

    std::int64_t max_population = START_POPULATION;
    std::int64_t max_resources = START_WHEAT +
        static_cast<std::int64_t>(START_LAND) * WHEAT_PER_ACRE_TO_BUY_UPPER;
    std::int64_t max_farmed_land = 0;
    std::int64_t previous_max_population = 0;

    for (int round = 0; round < rs.current_round; ++round) {
        previous_max_population = max_population;
        max_farmed_land = max_population * LAND_TO_FARM_PER_PERSON;
        max_resources = max_resources * WHEAT_PER_ACRE_TO_BUY_UPPER /
            WHEAT_PER_ACRE_TO_BUY_LOWER +
            max_farmed_land * (WHEAT_FROM_ACRE_UPPER - WHEAT_PER_ACRE_TO_SOW);
        max_population += NEW_POPULATION_UPPER;
    }

    const std::int64_t resources = rs.wheat +
        static_cast<std::int64_t>(rs.land) * WHEAT_PER_ACRE_TO_BUY_UPPER;
    if (rs.population > max_population || resources > max_resources ||
        static_cast<std::int64_t>(report.died_from_hunger_population) * 100 >
            previous_max_population * FAMINE_MIN_PERCENT_TO_LOSE ||
        report.wheat_eaten_by_rats > static_cast<std::int64_t>(
            static_cast<double>(max_resources) * WHEAT_EATEN_BY_RATS_COEF_UPPER)) {
        return false;
    }

    if (report.collected_wheat % report.wheat_from_acre != 0) {
        return false;
    }

    const int farmed_land = report.collected_wheat / report.wheat_from_acre;
    return farmed_land <= rs.land && farmed_land <= max_farmed_land;
}

RoundState create_initial_state() {
    return RoundState(
            START_POPULATION,
            START_LAND,
            START_WHEAT,
            0,
            0.0,
            get_random(WHEAT_PER_ACRE_TO_BUY_LOWER, WHEAT_PER_ACRE_TO_BUY_UPPER)
        );
}

GameOverReason update_state(RoundState &rs, const UpdateInfo &ui, RoundReport &r) {
    // random values

    r.wheat_from_acre = get_random(WHEAT_FROM_ACRE_LOWER, WHEAT_FROM_ACRE_UPPER);
    double wheat_eaten_by_rats_coef  = get_random(WHEAT_EATEN_BY_RATS_COEF_LOWER, WHEAT_EATEN_BY_RATS_COEF_UPPER);

    r.is_plague = get_random(0.0, 1.0) < PLAGUE_PROBABILITY;

    // 1. buy/sell land
    const std::int64_t trade = ui.land_to_buy_sell;
    const std::int64_t cost = trade * rs.acre_price;
    const std::int64_t land_after_trade = rs.land + trade;
    const std::int64_t wheat_after_trade = rs.wheat - cost;

    rs.land = static_cast<int>(land_after_trade);
    rs.wheat = static_cast<int>(wheat_after_trade);

    // 2. farming wheat
    r.collected_wheat = ui.land_to_farm * r.wheat_from_acre; // collect
    rs.wheat += ui.land_to_farm * (r.wheat_from_acre - WHEAT_PER_ACRE_TO_SOW);

    // 3. rats
    r.wheat_eaten_by_rats = static_cast<int>(rs.wheat * wheat_eaten_by_rats_coef);
    rs.wheat -= r.wheat_eaten_by_rats;

    // 4. hunger
    int wheat_to_eat = std::min(ui.wheat_to_eat, rs.wheat);
    rs.wheat -= wheat_to_eat;
    r.died_from_hunger_population = std::max(0, rs.population - wheat_to_eat / WHEAT_PER_PERSON);

    double current_year_hunger = 0.0;
    if (rs.population > 0) {
        current_year_hunger = r.died_from_hunger_population * 100.0 / rs.population;
    }
    int completed_rounds = rs.current_round + 1;
    rs.hunger_percent = (rs.hunger_percent * rs.current_round + current_year_hunger) / completed_rounds;

    if (current_year_hunger > FAMINE_MIN_PERCENT_TO_LOSE) {
        return GameOverReason::FAMINE_RIOT;
    }

    // 5. new population
    double new_population = std::floor(
        r.died_from_hunger_population / 2.0 + ((50.0 - r.wheat_from_acre) * rs.wheat) / 60000.0 + 1.0
        );
    r.new_population = static_cast<int>(std::clamp(
        new_population,
        static_cast<double>(NEW_POPULATION_LOWER),
        static_cast<double>(NEW_POPULATION_UPPER)));
    rs.population = rs.population - r.died_from_hunger_population + r.new_population;

    // 6. plague
    rs.population = static_cast<int>(std::floor(
        rs.population * (r.is_plague ? PLAGUE_POPULATION_DECLINE : 1.0)));

    if (rs.population <= 0) {
            return GameOverReason::EXTINCTION;
    }

    rs.acre_price = get_random(WHEAT_PER_ACRE_TO_BUY_LOWER, WHEAT_PER_ACRE_TO_BUY_UPPER);

    return GameOverReason::NONE;
}

double get_random(const double min, const double max) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(min, max);
    return dist(gen);
}

int get_random(const int min, const int max) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}
