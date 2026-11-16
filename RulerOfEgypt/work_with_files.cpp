#include "work_with_files.h"
#include <fstream>
#include <cstdio>
#include <iomanip>
#include <limits>

bool write_game_to_file(const RoundState &rs, const RoundReport &rch, const std::string& file_name) {
    std::ofstream out(file_name);
    if (!out.is_open()) {
        return false;
    }
    out << std::setprecision(std::numeric_limits<double>::max_digits10);

    out << rs.population << " " << rs.land << " " << rs.wheat << " "
        << rs.current_round << " " << rs.hunger_percent << " " << rs.acre_price << "\n";

    out << rch.died_from_hunger_population << " " << rch.new_population << " "
        << (rch.is_plague ? 1 : 0) << " " << rch.collected_wheat << " "
        << rch.wheat_from_acre << " " << rch.wheat_eaten_by_rats << "\n";

    out.close();
    return !out.fail();
}

bool read_game_from_file(RoundState &rs, RoundReport &rch, const std::string& file_name) {
    RoundState loaded_state{};
    RoundReport loaded_report{};

    std::ifstream in(file_name);
    if (!in.is_open()) {
        return false;
    }

    int is_plague = 0;
    if (!(in >> loaded_state.population >> loaded_state.land >> loaded_state.wheat
             >> loaded_state.current_round >> loaded_state.hunger_percent >> loaded_state.acre_price)) {
        return false;
    }

    if (!(in >> loaded_report.died_from_hunger_population >> loaded_report.new_population
             >> is_plague >> loaded_report.collected_wheat
             >> loaded_report.wheat_from_acre >> loaded_report.wheat_eaten_by_rats)) {
        return false;
    }

    if (is_plague != 0 && is_plague != 1) {
        return false;
    }

    in >> std::ws;
    if (!in.eof()) {
        return false;
    }

    loaded_report.is_plague = (is_plague != 0);

    if (!is_valid_saved_state(loaded_state, loaded_report)) {
        return false;
    }

    rs = loaded_state;
    rch = loaded_report;

    return true;
}

bool save_file_exists(const std::string& file_name) {
    std::ifstream in(file_name);
    return in.good();
}

void delete_save_file(const std::string& file_name) {
    std::remove(file_name.c_str());
}
