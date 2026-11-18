//
// Created by gonch2 on 26.10.2026.
//

#ifndef RULEROFEGYPT_WORK_WITH_FILES_H
#define RULEROFEGYPT_WORK_WITH_FILES_H
#include <string>

#include "round_state.h"
#include "settings.h"

bool WriteGameToFile(const RoundState &rs, const RoundReport &rch, const std::string& file_name = kFileName);
bool ReadGameFromFile(RoundState &rs, RoundReport &rch, const std::string& file_name = kFileName);
bool SaveFileExists(const std::string& file_name = kFileName);
void DeleteSaveFile(const std::string& file_name = kFileName);

#endif //RULEROFEGYPT_WORK_WITH_FILES_H
