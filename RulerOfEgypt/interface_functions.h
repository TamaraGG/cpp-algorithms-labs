//
// Created by gonch2 on 13.10.2026.
//

#ifndef RULEROFEGYPT_IO_FUNCTIONS_H
#define RULEROFEGYPT_IO_FUNCTIONS_H

#include <string>

#include "round_state.h"

void PrintFirstReport(const RoundState& rs);
void PrintReport(const RoundState& rs, const RoundReport& rch);
std::string PrintWheatBushels(int amount);
std::string PrintLandAcres(int amount);
std::string print_people(int amount);
void GetInfoFromUser(UpdateInfo &ui, const RoundState& rs, const RoundReport& rr);
void PrintErrors(const std::vector<ValidationError> &errors);
void AskPlayer(UpdateInfo& ui);
void PrintFinalStatistics(const RoundState& rs);
void PrintLossReport(GameOverReason reason);

int ReadInt(const std::string& prompt, bool is_none_negative = false);
bool AskLoadSavedGame();
bool AskSaveAndExit();
bool AskExitWithoutSave();

#endif //RULEROFEGYPT_IO_FUNCTIONS_H
