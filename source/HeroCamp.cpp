#include "HeroCamp.h"

#include <QtCore/QString>

#include "StreamUtils.h"
#include "GameState.h"
#include "Hero.h"
#include "SpellCardStorage.h"
#include "SaveContext.h"
#include "LoadContext.h"

using namespace std;

HeroCamp* HeroCamp::instance = 0;

HeroCamp::HeroCamp()
: _heroes()
{
    instance = this;
}

HeroCamp::~HeroCamp()
{
    deleteHeroes();
}

/*!
 * Deletes each single hero, then clears the _heroes vector.
 */
void HeroCamp::deleteHeroes()
{
    for (vector<Hero*>::iterator it = _heroes.begin(); it != _heroes.end(); ++it)
    {
        delete *it;
    }
    _heroes.clear();
}

/*!
 * Clears _heroes, then creates the heroes given by hero_names, assigns spell familied and spells,
 * and adds them to _heroes.
 *
 * @param hero_names
 * @param alb_spell_family
 */
void HeroCamp::createHeroes(const vector<QString>& hero_names, SpellCard::SpellFamily alb_spell_family)
{
    deleteHeroes();

    for (vector<QString>::const_iterator it = hero_names.begin(); it != hero_names.end(); ++it)
    {
        const QString& hero_name = *it;

        if (hero_name == "Barbarian")
            addHero<Barbarian>();
        else if (hero_name == "Dwarf")
            addHero<Dwarf>();
        else if (hero_name == "Alb")
        {
            Hero* alb = addHero<Alb>();
            deduceAndSetSpellFamilies(alb, alb_spell_family);
        }
        else if (hero_name == "Magician")
        {
            Hero* magician = addHero<Magician>();
            deduceAndSetSpellFamilies(magician, alb_spell_family);
        }
        else
        {
            DVX(("unknown hero name \"%s\"", qPrintable(hero_name)));
            continue;
        }
    }

    redistributeSpellCardsToHeroes();
}

/*!
 * Uses the SpellCardStorage to redistribute the spell cards to the heroes according to the spell families
 * set at the heroes.
 */
void HeroCamp::redistributeSpellCardsToHeroes()
{
    vector<SpellCard>& global_spell_card_stock = SpellCardStorage::instance->getSpellCardStock();
    vector<SpellCard>& global_spell_card_deposition = SpellCardStorage::instance->getSpellCardDeposition();

    // 1. move all spell cards from heroes back onto the spell card stock
    for (vector<Hero*>::iterator it = _heroes.begin(); it != _heroes.end(); ++it)
    {
        Hero* hero = *it;
        const set<SpellCard>& hero_spell_cards = hero->getInventory().getSpellCards();
        for (set<SpellCard>::const_iterator it_const = hero_spell_cards.begin(); it_const != hero_spell_cards.end();
                ++it_const)
        {
            global_spell_card_stock.push_back(*it_const); // copies the spell card
        }
        hero->getInventory().removeSpellCards();
    }

    // 2. put all spell cards from spell card deposition back onto the spell card stock
    for (vector<SpellCard>::const_iterator it = global_spell_card_deposition.begin();
            it != global_spell_card_deposition.end(); ++it)
    {
        global_spell_card_stock.push_back(*it); // copies the spell card
    }
    global_spell_card_deposition.clear();

    // 3. redistribute the spell cards to the respective heroes
    for (vector<Hero*>::iterator it = _heroes.begin(); it != _heroes.end(); ++it)
    {
        Hero* hero = *it;
        hero->obtainSpellCardsFromStorage();
    }
}

/*!
 * @param heroes is filled with _heroes.
 */
void HeroCamp::getHeroes(vector<Hero*>* heroes)
{
    heroes->clear();
    for (vector<Hero*>::iterator it = _heroes.begin(); it != _heroes.end(); ++it)
    {
        heroes->push_back(*it);
    }
}

/*!
 * @return True if at least one hero has more than 0 gold pieces; false otherwise.
 */
bool HeroCamp::heroesCanBuyEquipment() const
{
    for (vector<Hero*>::const_iterator it = _heroes.begin(); it != _heroes.end(); ++it)
    {
        const Hero* hero = *it;
        if (hero->getGold() > 0)
            return true;
    }

    return false;
}

bool HeroCamp::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "HeroCamp");
    bool result = true;

    for (uint i = 0; i < _heroes.size(); ++i)
    {
        QString i_str;
        i_str.setNum(i);
        SaveContext::OpenChapter open_chapter_hero(save_context, QString("_heroes[") + i_str + "]");

        save_context.writeString(_heroes[i]->getName(), "name");

        if (!_heroes[i]->save(save_context))
            result = false;
    }

    return result;
}

bool HeroCamp::load(LoadContext& load_context, const GameState& game_state)
{
    LoadContext::OpenChapter open_chapter(load_context, "HeroCamp");

    DV(("num_heroes: %d", game_state._hero_names.size()));

    // recreate heroes
    createHeroes(game_state._hero_names, game_state._alb_spell_family);

    for (uint i = 0; i < game_state._hero_names.size(); ++i)
    {
        QString hero_name;
        QString i_str;
        i_str.setNum(i);
        if (!load_context.readString(&hero_name, QString("_heroes[") + i_str + "].name"))
            return false;

        if (!_heroes[i]->load(load_context))
            return false;
    }

    return true;
}

/*!
 * @param hero
 * @param alb_spell_family
 */
void HeroCamp::deduceAndSetSpellFamilies(Hero* hero, SpellCard::SpellFamily alb_spell_family)
{
    if (hero->getName().compare(Alb::className()) == 0)
    {
        hero->addSpellFamily(alb_spell_family);
    }
    else if (hero->getName().compare(Magician::className()) == 0)
    {
        for (uint i = 0; i < (uint) SpellCard::SPELL_FAMILY_UNKNOWN; ++i)
        {
            if (i != (uint) alb_spell_family)
            {
                hero->addSpellFamily((SpellCard::SpellFamily) i);
            }
        }
    }
}
