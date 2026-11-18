#include <iostream>
#include <windows.h>

#include "interface_functions.h"
#include "round_state.h"
#include "settings.h"
#include "work_with_files.h"


int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    RoundState round_state = CreateInitialState();
    UpdateInfo update_info{};
    RoundReport round_changes{};

    if (SaveFileExists(kFileName)) {
        if (AskLoadSavedGame()) {
            if (ReadGameFromFile(round_state, round_changes, kFileName)) {
                std::cout << "Игра успешно загружена!\n";
            } else {
                std::cout << "Не удалось прочитать сохранение. Начинаем новую игру.\n";
                round_state = CreateInitialState();
                DeleteSaveFile(kFileName);
            }
        }
    }

    for (; round_state.current_round < kRoundNumber; round_state.current_round++) {
        if (round_state.current_round == 0) {
            PrintFirstReport(round_state);
        } else {
            PrintReport(round_state, round_changes);
        }

        if (AskSaveAndExit()) {
            if (WriteGameToFile(round_state, round_changes, kFileName)) {
                std::cout << "Игра успешно сохранена. До скорой встречи, повелитель!\n";
                return 0;
            }
            std::cout << "Приношу свои извинения, но игра не может быть сохранена.";
            if (AskExitWithoutSave()) {
                std::cout << "Прощайте, повелитель!\n";
                return 0;
            }

        }

        GetInfoFromUser(update_info, round_state, round_changes);
        GameOverReason update_status = UpdateState(round_state, update_info, round_changes);

        if (update_status != GameOverReason::kNone) {
            PrintLossReport(update_status);
            break;
        }
    }

    if (round_state.current_round == kRoundNumber) {
        PrintFinalStatistics(round_state);
    }

    return 0;
}
