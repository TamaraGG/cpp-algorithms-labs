//
// Created by gonch2 on 26.10.2026.
//

#ifndef RULEROFEGYPT_WORK_WITH_FILES_H
#define RULEROFEGYPT_WORK_WITH_FILES_H
#include <string>

#include "round_state.h"
#include "settings.h"

bool write_game_to_file(const RoundState &rs, const RoundReport &rch, const std::string& file_name = FILE_NAME);
bool read_game_from_file(RoundState &rs, RoundReport &rch, const std::string& file_name = FILE_NAME);
bool save_file_exists(const std::string& file_name = FILE_NAME);
void delete_save_file(const std::string& file_name = FILE_NAME);

#endif //RULEROFEGYPT_WORK_WITH_FILES_H
