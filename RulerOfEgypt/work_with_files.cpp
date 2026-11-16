#include "work_with_files.h"
#include <fstream>
#include <cstdio>

bool write_game_to_file(const RoundState &rs, const RoundReport &rch, const std::string& file_name) {
    std::ofstream out(file_name);
    if (!out.is_open()) {
        return false;
    }

    out << rs.population << " " << rs.land << " " << rs.wheat << " "
        << rs.current_round << " " << rs.hunger_percent << " " << rs.acre_price << "\n";

    out << rch.died_from_hunger_population << " " << rch.new_population << " "
        << (rch.is_plague ? 1 : 0) << " " << rch.collected_wheat << " "
        << rch.wheat_from_acre << " " << rch.wheat_eaten_by_rats << "\n";

    return out.good();
}

bool read_game_from_file(RoundState &rs, RoundReport &rch, const std::string& file_name) {
    std::ifstream in(file_name);
    if (!in.is_open()) {
        return false;
    }

    int is_plague = 0;
    if (!(in >> rs.population >> rs.land >> rs.wheat
             >> rs.current_round >> rs.hunger_percent >> rs.acre_price)) {
        return false;
    }

    if (!(in >> rch.died_from_hunger_population >> rch.new_population
             >> is_plague >> rch.collected_wheat
             >> rch.wheat_from_acre >> rch.wheat_eaten_by_rats)) {
        return false;
    }

    rch.is_plague = (is_plague != 0);
    return true;
}

bool save_file_exists(const std::string& file_name) {
    std::ifstream in(file_name);
    return in.good();
}

void delete_save_file(const std::string& file_name) {
    std::remove(file_name.c_str());
}
