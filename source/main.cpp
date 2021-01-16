
#define HQ_VERSION "0.3"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>

#include <QtGui/QGuiApplication>
#include <QtWidgets/QStyleFactory>
#include <QtWidgets/QFileDialog>
#include <QtCore/QResource>

#include "HeroQuestStartDialog.h"
#include "HeroQuestNewGameDialog.h"
#include "HeroQuestLevelWindow.h"
#include "LevelTheProbation.h"
#include "GameState.h"
#include "SpellCardStorage.h"
#include "Debug.h"
#include "DialogLevelBriefing.h"

using namespace std;

/*
 * TODO:
 * - Spells implementieren, wo noch was fehlt
 * - wenn das Ziel der Mission erreicht ist, muss ein "geschafft"-Dialog kommen, und das nächste Level beginnt!
 *   => im Level muss irgendwie eine Success-Condition definiert werden!
 * - mit SonarGraph o.Ä. eine Grafik bauen, welche Klasse mit welcher anderen redet
 * - Dialog Buy Equipment
 */
int main(int argc, char* argv[])
{
#if 0
    string a;
    cin >> a;
#endif

    // initialize random seed
    std::srand(std::time(NULL));

    QApplication app(argc, argv);

    QResource::registerResource("resources/qrc_application.rcc");

    SpellCardStorage spell_card_storage;
    if (!spell_card_storage.loadSpellCards())
        ProgError("Could not load spell cards");

    bool continue_playing = true;
    while (continue_playing)
    {
        GameState game_state;

        // TODO: automatically insert all existing levels' names.
        // Note that currently saved games do contain only the current level's name, no list of all (used) levels.
        game_state._level_names.push_back(LevelTheProbation::prettyClassName());

        bool game_start_conditions_complete = false;

        // only for load game
        QString filename;

        // let the user choose the game mode (new, quick, load etc.)
        HeroQuestStartDialog::DialogReturnValue start_dialog_return_value;
        while (!game_start_conditions_complete && continue_playing)
        {
            filename = "";

            HeroQuestStartDialog hero_quest_start_dialog(&start_dialog_return_value);
            hero_quest_start_dialog.exec();

            switch (start_dialog_return_value)
            {
                case HeroQuestStartDialog::NEW_GAME:
                {
                    HeroQuestNewGameDialog::DialogReturnValue new_game_dialog_return_value;
                    HeroQuestNewGameDialog new_game_start_dialog(&new_game_dialog_return_value, &(game_state._hero_names), &(game_state._alb_spell_family));
                    new_game_start_dialog.exec();

                    if (new_game_dialog_return_value == HeroQuestNewGameDialog::QUIT)
                        break;

                    // HeroQuestNewGameDialog::OK

                    game_start_conditions_complete = true;
                }
                break;

                case HeroQuestStartDialog::LOAD_GAME:
                {
                    if (!HeroQuestLevelWindow::chooseLoadGameFilename(0, &filename))
                    {
                        // cancel loading and start over
                        continue;
                    }

                    ifstream infile(filename.toUtf8().constData(), ios::binary);

                    if (!game_state.load(infile))
                    {
                        // loading the game suffered from some error => cancel loading and start over
                        continue;
                    }

                    game_start_conditions_complete = true;
                }
                break;

                case HeroQuestStartDialog::QUICK_GAME:
                    game_state._hero_names.push_back(Barbarian::className());
                    game_state._hero_names.push_back(Dwarf::className());
                    game_state._hero_names.push_back(Alb::className());
                    game_state._hero_names.push_back(Magician::className());

                    // default: Alb gets air spells pile
                    game_state._alb_spell_family = SpellCard::AIR;

                    game_start_conditions_complete = true;
                    break;

                case HeroQuestStartDialog::QUIT:
                    continue_playing = false;
                    break;
            }
        }

        if (!continue_playing)
            break;

        // adjust level to start from
        if (start_dialog_return_value != HeroQuestStartDialog::LOAD_GAME)
        {
            game_state._current_level = GameState::LEVEL_THE_PROBATION;
        }

        // loop over the levels to play
        for (uint i_level = (uint) (game_state._current_level); i_level < game_state._level_names.size(); ++i_level)
        {
            // briefing
            DialogLevelBriefing dialog_level_briefing((GameState::LevelID) i_level);
            dialog_level_briefing.exec();

            /*
             TODO:
             Buy Equipment Dialog shows up (if at least 1 hero has > 0 gold pieces)
             - choose hero / show inventory (no "use" buttons)
             - available equipment overview -> "buy" buttons
             */

            // for now: normal operation mode
            game_state._current_level_state = GameState::RUNNING;

            HeroQuestLevelWindow hero_quest_level_window(HQ_VERSION, app, filename, &game_state, &spell_card_storage);
            hero_quest_level_window.show();

            // start global event loop
            int exit_code = app.exec();
            if (exit_code == HeroQuestLevelWindow::EXIT_CODE_LOST)
            {
                // TODO: show "lost" dialog
                break;
            }
            else
            {
                // TODO: show "success" dialog
            }
        }
    }

    return 0;
}

