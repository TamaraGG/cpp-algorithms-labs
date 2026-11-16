#include <iostream>
#include <windows.h>

#include "interface_functions.h"
#include "round_state.h"
#include "settings.h"
#include "work_with_files.h"

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    RoundState round_state = create_initial_state();
    UpdateInfo update_info{};
    RoundReport round_changes{};

    bool loaded = false;
    if (save_file_exists(FILE_NAME)) {
        if (ask_load_saved_game()) {
            if (read_game_from_file(round_state, round_changes, FILE_NAME)) {
                loaded = true;
                std::cout << "Игра успешно загружена!\n";
                if (round_state.current_round > 0) {
                    RoundState temp = round_state;
                    temp.current_round--;
                    print_report(temp, round_changes);
                } else {
                    print_first_report(round_state);
                }
            } else {
                std::cout << "Не удалось прочитать сохранение. Начинаем новую игру.\n";
                round_state = create_initial_state();
            }
        } else {
            delete_save_file(FILE_NAME);
        }
    }

    if (!loaded) {
        print_first_report(round_state);
    }

    for (; round_state.current_round < ROUNDS_NUMBER; round_state.current_round++) {
        if (ask_save_and_exit()) {
            write_game_to_file(round_state, round_changes, FILE_NAME);
            std::cout << "Игра успешно сохранена. До скорой встречи, повелитель!\n";
            return 0;
        }

        get_info_from_user(update_info, round_state, round_changes);
        GameOverReason update_status = update_state(round_state, update_info, round_changes);

        if (update_status != GameOverReason::NONE) {
            print_loss_report(update_status);
            delete_save_file(FILE_NAME);
            break;
        }

        print_report(round_state, round_changes);
    }

    if (round_state.current_round == ROUNDS_NUMBER) {
        print_final_statistics(round_state);
        delete_save_file(FILE_NAME);
    }

    return 0;
}
