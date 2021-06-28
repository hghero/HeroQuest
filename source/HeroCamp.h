#ifndef HERO_CAMP_H
#define HERO_CAMP_H

#include <vector>
#include <fstream>
#include <iostream>
#include <QtCore/QString>

#include "Hero.h"
#include "Debug.h"

struct GameState;
class SaveContext;
class LoadContext;

class HeroCamp
{
public:
    HeroCamp();
    virtual ~HeroCamp();

    void deleteHeroes();
    void createHeroes(const std::vector<QString>& hero_names, SpellCard::SpellFamily alb_spell_family);

    void getHeroes(std::vector<Hero*>* heroes);
    bool heroesCanBuyEquipment() const;

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context, const GameState& game_state);

    void redistributeSpellCardsToHeroes();

    static HeroCamp* instance;

private:
    std::vector<Hero*> _heroes;

    /*!
     * Adds a hero of type HeroType, and returns the added hero.
     *
     * @param alb_spell_family
     */
    template<class HeroType>
    Hero* addHero()
    {
        // make sure that no hero of this type exists yet
        for (std::vector<Hero*>::const_iterator it = _heroes.begin(); it != _heroes.end(); ++it)
        {
            if ((*it)->getName().compare(HeroType::className()) == 0)
            {
                DVX(("Hero of type \"%s\" already exists in the HeroCamp!", qPrintable((*it)->getName())));
                return 0;
            }
        }

        Hero* hero = new HeroType();
        hero->create();
        //hero->getInventory().addGold(1000);
        _heroes.push_back(hero);
        return _heroes.back();
    }

    /*!
     * @param alb_spell_family
     */
    void deduceAndSetSpellFamilies(Hero* hero, SpellCard::SpellFamily alb_spell_family);
};

#endif
