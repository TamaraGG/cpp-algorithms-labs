//
// Created by gonch2 on 13.10.2026.
//
#include <iostream>
#include <iomanip>
#include "interface_functions.h"

#include <cmath>
#include <cstdlib>
#include <limits>
#include <sstream>

void PrintFirstReport(const RoundState &rs) {
    std::cout << "Мой повелитель, с данного момента Вам предстоит править нашим государством Египтом.\n"
                 "Сейчас идет год " << rs.current_round + 1 << " твоего высочайшего правления.\n"
                 "Позвольте мне доложить о текущем положении дел:\n"
                 "\tВам принадлежит " << PrintLandAcres(rs.land) << " земли.\n"
                 "\tВсего в государстве проживает " << print_people(rs.population) << ".\n"
                 "\tНа данный момент в запасах имеется " << PrintWheatBushels(rs.wheat) <<
                     " пшеницы.\n"
                 "\tЦена одного акра земли составляет " <<
                     PrintWheatBushels(rs.acre_price) << " пшеницы.\n";
}

void PrintReport(const RoundState& rs, const RoundReport& rch) {

    std::cout << "------------------------------------------" << std::endl;

    std::cout << "Мой повелитель, соизволь поведать тебе" << std::endl;

    std::cout << "в году " << rs.current_round + 1 << " твоего высочайшего правления" << std::endl;

    if (rch.died_from_hunger_population != 0)
        std::cout << print_people(rch.died_from_hunger_population) << " умерли с голоду.\n";
    if (rch.new_population != 0)
        std::cout << print_people(rch.new_population) << " прибыли в наш великий город.\n" << std::endl;
    if (rch.is_plague)
        std::cout << "Чума уничтожила половину населения;" << std::endl;
    std::cout << "Население города сейчас составляет " << print_people(rs.population) << ";" << std::endl;

    std::cout << "Мы собрали "
        << PrintWheatBushels(rch.collected_wheat) << " пшеницы, по ";
    std::cout << PrintWheatBushels(rch.wheat_from_acre) << " с акра;" << std::endl;
    if (rch.wheat_eaten_by_rats != 0)
        std::cout << "Крысы истребили " << PrintWheatBushels(rch.wheat_eaten_by_rats) << " пшеницы, оставив "
            << PrintWheatBushels(rs.wheat) << " в амбарах;" << std::endl;
    else
        std::cout << "Сейчас у вас " << PrintWheatBushels(rs.wheat) << " в амбарах;" << std::endl;

    std::cout << "1 акр земли стоит сейчас " << PrintWheatBushels(rs.acre_price) << std::endl;

    std::cout << "Город сейчас занимает " << PrintLandAcres(rs.land) << "." << std::endl;

    std::cout << "------------------------------------------" << std::endl;

}

std::string PrintWheatBushels(int amount) {
    const bool is_whole = amount % 10 == 0;
    const int last_two_digits = std::abs((amount / 10) % 100);
    const int last_digit = last_two_digits % 10;
    const char* word = "бушелей";

    if (!is_whole) {
        word = "бушеля";
    } else if (last_two_digits < 11 || last_two_digits > 14) {
        if (last_digit == 1) {
            word = "бушель";
        } else if (last_digit >= 2 && last_digit <= 4) {
            word = "бушеля";
        }
    }

    std::ostringstream output;
    output << std::fixed << std::setprecision(is_whole ? 0 : 1)
           << static_cast<double>(amount) / 10.0 << ' ' << word;
    return output.str();
}

std::string PrintLandAcres(int amount) {
    const int last_two_digits = std::abs(amount % 100);
    const int last_digit = last_two_digits % 10;
    const char* word = "акров";

    if (last_two_digits < 11 || last_two_digits > 14) {
        if (last_digit == 1) {
            word = "акр";
        } else if (last_digit >= 2 && last_digit <= 4) {
            word = "акра";
        }
    }
    return std::to_string(amount) + ' ' + word;
}

std::string print_people(int amount) {
    const int last_two_digits = std::abs(amount % 100);
    const int last_digit = last_two_digits % 10;
    const char* word = "человек";

    if ((last_two_digits < 11 || last_two_digits > 14) &&
        last_digit >= 2 && last_digit <= 4) {
        word = "человека";
    }
    return std::to_string(amount) + ' ' + word;
}

int ReadInt(const std::string& prompt, bool is_none_negative) {
    while (true) {
        std::cout << prompt;

        std::string line;
        if (!std::getline(std::cin, line)) {
            std::exit(0);
        }

        std::istringstream input(line);
        int value;
        char extra;

        if (!(input >> value) || (input >> extra)) {
            std::cout << "О повелитель, соизволь ввести целое число цифрами!\n";
            continue;
        }

        if (is_none_negative && value < 0) {
            std::cout << "Количество пшеницы не может быть отрицательным.\n";
            continue;
        }

        return value;
    }
}

void AskPlayer(UpdateInfo& ui) {
    std::cout << "\nЧто пожелаете, повелитель?\n";

    ui.land_to_buy_sell = ReadInt("\tСколько акров земли повелеваешь купить (положительное число) или продать (отрицательное число)? ");

    while (true) {
        int bushels = ReadInt("\tСколько бушелей пшеницы повелеваешь съесть? ", true);
        if (bushels > std::numeric_limits<int>::max() / 10) {
            std::cout << "Слишком большое число. Повторите ввод.\n";
            continue;
        }
        ui.wheat_to_eat = bushels * 10;
        break;
    }

    ui.land_to_farm = ReadInt("\tСколько акров земли повелеваешь засеять? ", true);
}

void PrintFinalStatistics(const RoundState &rs) {

    const std::streamsize previous_precision = std::cout.precision();
    const std::ios_base::fmtflags previous_flags = std::cout.flags();

    double land_per_person = (rs.population > 0) ? (static_cast<double>(rs.land) / rs.population) : 0.0;
    int hunger_percent = static_cast<int>(std::round(rs.hunger_percent));

    std::cout << "Мой повелитель, Вы завершили свое правление, длившееся 10 лет.\n"
                 "В результате Вашего правления:\n"
                 "\tсреднегодовой процент умерших от голода людей составил "
                 << hunger_percent << "%.\n"
                 "\tКоличество акров земли на одного жителя составило "
                 << std::fixed << std::setprecision(2) << land_per_person << "\n";

    std::cout.precision(previous_precision);
    std::cout.flags(previous_flags);

    if (land_per_person < 7 and rs.hunger_percent > 33) {
        std::cout << "Из-за вашей некомпетентности в управлении, народ устроил бунт, и изгнал вас их города. "
                     "Теперь вы вынуждены влачить жалкое существование в изгнании\n";
    } else if (land_per_person < 9 and rs.hunger_percent > 10) {
        std::cout << "Вы правили железной рукой, подобно Нерону и Ивану Грозному. Народ вздохнул с облегчением, "
                     "и никто больше не желает видеть вас правителем\n";
    } else if (land_per_person < 10 and rs.hunger_percent > 3) {
        std::cout << "Вы справились вполне неплохо, у вас, конечно, есть недоброжелатели, "
                     "но многие хотели бы увидеть вас во главе города снова\n";
    } else {
        std::cout << "Фантастика! Карл Великий, Дизраэли и Джефферсон вместе не справились бы лучше\n";
    }
}

void PrintLossReport(GameOverReason reason) {
    switch (reason) {
        case GameOverReason::kExtinction:
            std::cout << "В этом году все в городе умерли абсолютно все люди. "
                         "Вы не справились с правлением. Позор Вам.\n";
            break;
        case GameOverReason::kFamineRiot:
            std::cout << "В этом году от голода умерло слишком много людей. "
                         "Вы плохо распоряжаетесь ресурсами. Позор Вам.\n";
        case GameOverReason::kNone:
            break;
    }
}

void GetInfoFromUser(UpdateInfo &ui, const RoundState& rs, const RoundReport& rr) {

    std::vector<ValidationError> errors;

    do {
        AskPlayer(ui);

        errors = ValidateUpdateInfo(ui, rs);

        if (errors.empty()) {
            std::cout << "Как пожелаете, господин. Все будет исполнено.\n";
            break;
        }
        PrintErrors(errors);
    } while (true);

}

void PrintErrors(const std::vector<ValidationError> &errors) {
    for (auto& error : errors) {
        switch (error) {
            case ValidationError::kNotEnoughLandToSell:
                std::cout << "У нас недостаточно земли, чтобы продать столько.\n";
                break;
            case ValidationError::kNotEnoughWheatToSell:
                std::cout << "У нас недостаточно пшеницы, чтобы купить столько земли.\n";
                break;
            case ValidationError::kNegativeValue:
                std::cout << "Извините, но количество пшеницы и/или количество земли для обработки не может "
                             "быть отрицательным.\n";
                break;
            case ValidationError::kNotEnoughWheatToEat:
                std::cout << "В закромах нет столько пшеницы, чтобы выделить ее на еду.\n";
                break;
            case ValidationError::kNotEnoughLandToFarming:
                std::cout << "У нас будет недостаточно земли, чтобы засеять столько земли.\n";
                break;
            case ValidationError::kNotEnoughPeopleForFarming:
                std::cout << "В нашем государстве людей слишком мало, чтобы работать на таком количестве земли.\n";
                break;
            case ValidationError::kNotEnoughWheatToFarm:
                std::cout << "К сожалению, у нас не будет достаточно пшеницы, чтобы засеять столько земли.\n";
                break;
        }
    }
}

bool AskLoadSavedGame() {
    std::cout << "Найдена сохраненная игра. Желаете продолжить (1) или начать заново (0)? ";
    int choice = -1;
    while (choice != 0 && choice != 1) {
        choice = ReadInt("");
    }
    return choice == 1;
}

bool AskSaveAndExit() {
    std::cout << "\nЖелаете продолжить игру (1) или сохраниться и выйти (0)? ";
    int choice = -1;
    while (choice != 0 && choice != 1) {
        choice = ReadInt("");
    }
    return choice == 0;
}

bool AskExitWithoutSave() {
    std::cout << "\nЖелаете продолжить игру (1) или выйти без сохранения (0)? ";
    int choice = -1;
    while (choice != 0 && choice != 1) {
        choice = ReadInt("");
    }
    return choice == 0;
}
