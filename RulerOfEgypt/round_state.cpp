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
                                                  const RoundState &rs,
                                                  const RoundReport &rr) {
    std::vector<ValidationError> errors;

    if (ui.wheat_to_eat < 0 or ui.land_to_farm < 0) {
        errors.push_back(ValidationError::NEGATIVE_VALUE);
    }

    if (ui.land_to_buy_sell * rs.acre_price > rs.wheat) {
        errors.push_back(ValidationError::NOT_ENOUGH_WHEAT_TO_SELL);
    }
    else if (-ui.land_to_buy_sell > rs.land) {
        errors.push_back(ValidationError::NOT_ENOUGH_LAND_TO_SELL);

    }
    else {
        if (ui.wheat_to_eat > rs.wheat - ui.land_to_buy_sell * rs.acre_price) {
            errors.push_back(ValidationError::NOT_ENOUGH_WHEAT_TO_EAT);
        }

        int new_land = std::max(0, rs.land + ui.land_to_buy_sell);
        int new_wheat = std::max(0, rs.wheat - ui.wheat_to_eat - ui.land_to_buy_sell * rs.acre_price);

        if (ui.land_to_farm > new_land) {
            errors.push_back(ValidationError::NOT_ENOUGH_LAND_TO_FARM);
        }
        else if (ui.land_to_farm > rs.population * LAND_TO_FARM_PER_PERSON) {
            errors.push_back(ValidationError::NOT_ENOUGH_PEOPLE_FOR_FARM);
        }
        else if (ui.land_to_farm * WHEAT_PER_ACRE_TO_SOW > new_wheat) {
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
        !std::isfinite(rs.hunger_percent) || rs.hunger_percent < 0.0f ||
        rs.hunger_percent > FAMINE_MIN_PERCENT_TO_LOSE) {
        return false;
    }

    if (rs.current_round == 0) {
        return rs.population == START_POPULATION && rs.land == START_LAND &&
            rs.wheat == START_WHEAT && rs.hunger_percent == 0.0f &&
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
            max_resources * static_cast<double>(WHEAT_EATEN_BY_RATS_COEF_UPPER))) {
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
            0,
            get_random(WHEAT_PER_ACRE_TO_BUY_LOWER, WHEAT_PER_ACRE_TO_BUY_UPPER)
        );
}

GameOverReason update_state(RoundState &rs, const UpdateInfo &ui, RoundReport &r) {
    // random values

    r.wheat_from_acre = get_random(WHEAT_FROM_ACRE_LOWER, WHEAT_FROM_ACRE_UPPER);
    float wheat_eaten_by_rats_coef  = get_random(WHEAT_EATEN_BY_RATS_COEF_LOWER, WHEAT_EATEN_BY_RATS_COEF_UPPER);

    r.is_plague = get_random(0.f, 1.f) < PLAGUE_PROBABILITY;

    // 1. buy/sell land
    rs.land += ui.land_to_buy_sell;
    rs.wheat = std::max(0, rs.wheat - ui.land_to_buy_sell * rs.acre_price);

    // 2. farming wheat
    r.collected_wheat = ui.land_to_farm * r.wheat_from_acre; // collect
    rs.wheat += ui.land_to_farm * (r.wheat_from_acre - WHEAT_PER_ACRE_TO_SOW);

    // 3. rats
    r.wheat_eaten_by_rats = static_cast<int>(static_cast<double>(rs.wheat) * wheat_eaten_by_rats_coef);
    rs.wheat -= r.wheat_eaten_by_rats;

    // 4. hunger
    int wheat_to_eat = std::min(ui.wheat_to_eat, rs.wheat);
    rs.wheat -= wheat_to_eat;
    r.died_from_hunger_population = std::max(0, rs.population - wheat_to_eat / WHEAT_PER_PERSON);

    float current_year_hunger = 0.0f;
    if (rs.population > 0) {
        current_year_hunger = (static_cast<float>(r.died_from_hunger_population) / static_cast<float>(rs.population)) * 100.0f;
    }
    int completed_rounds = rs.current_round + 1;
    rs.hunger_percent = (rs.hunger_percent * static_cast<float>(rs.current_round) + current_year_hunger) /
        static_cast<float>(completed_rounds);

    if (current_year_hunger > FAMINE_MIN_PERCENT_TO_LOSE) {
        return GameOverReason::FAMINE_RIOT;
    }

    // 5. new population
    int64_t new_population = std::floor(
        r.died_from_hunger_population / 2.0 + ((50 - r.wheat_from_acre) * rs.wheat) / 60000.0 + 1.0
        );
    r.new_population = static_cast<int>(new_population);
    r.new_population = std::clamp(r.new_population, NEW_POPULATION_LOWER, NEW_POPULATION_UPPER);
    rs.population = rs.population - r.died_from_hunger_population + r.new_population;

    // 6. plague
    rs.population = std::floor(rs.population * (r.is_plague ? PLAGUE_POPULATION_DECLINE : 1.));

    if (rs.population <= 0) {
            return GameOverReason::EXTINCTION;
    }

    rs.acre_price = get_random(WHEAT_PER_ACRE_TO_BUY_LOWER, WHEAT_PER_ACRE_TO_BUY_UPPER);

    return GameOverReason::NONE;
}

float get_random(const float min, const float max) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

int get_random(const int min, const int max) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}
