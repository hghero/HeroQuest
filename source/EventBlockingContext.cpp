//#define HG_DEBUG

#include "EventBlockingContext.h"
#include "HeroQuestLevelWindow.h"

/*!
 * If multiple instances exist simultaneously, they must be strictly hierarchic!
 */
EventBlockingContext::EventBlockingContext(HeroQuestLevelWindow* hero_quest_level_window,
        const EventBlockingBitmask event_blocking_bitmask)
        :
        _hero_quest_level_window(hero_quest_level_window), _previous_event_blocking_bitmask(
                hero_quest_level_window->getEventBlockingBitmask()), _event_blocking_bitmask(event_blocking_bitmask)
{
    _hero_quest_level_window->setEventBlockingBitmask(_event_blocking_bitmask);
}

EventBlockingContext::~EventBlockingContext()
{
    _hero_quest_level_window->setEventBlockingBitmask(_previous_event_blocking_bitmask);
}
