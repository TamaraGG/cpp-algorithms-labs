//
// Created by gonch2 on 13.10.2026.
//

#ifndef RULEROFEGYPT_IO_FUNCTIONS_H
#define RULEROFEGYPT_IO_FUNCTIONS_H

#include <string>

#include "round_state.h"

void print_first_report(const RoundState& rs);
void print_report(const RoundState& rs, const RoundReport& rch);
std::string print_wheat_bushels(int amount);
std::string print_land_acres(int amount);
std::string print_people(int amount);
void get_info_from_user(UpdateInfo &ui, const RoundState& rs, const RoundReport& rr);
void print_errors(const std::vector<ValidationError> &errors);
void ask_player(UpdateInfo& ui);
void print_final_statistics(const RoundState& rs);
void print_loss_report(GameOverReason reason);

int read_int(const std::string& prompt, bool is_none_negative = false);
bool ask_load_saved_game();
bool ask_save_and_exit();
bool ask_exit_without_save();

#endif //RULEROFEGYPT_IO_FUNCTIONS_H
