#ifndef EVENTBLOCKINGCONTEXT_H
#define EVENTBLOCKINGCONTEXT_H

#include <QtCore/QString>

class HeroQuestLevelWindow;

// ==================================================================

class EventBlockingContext
{
    public:
    typedef uint EventBlockingBitmask;
    static const EventBlockingBitmask EVENT_BLOCKING_NONE = 0;
    static const EventBlockingBitmask EVENT_BLOCKING_PAINT_EVENTS = 1;
    static const EventBlockingBitmask EVENT_BLOCKING_INPUT_EVENTS = 2;

    EventBlockingContext(HeroQuestLevelWindow* hero_quest_level_window,
            const EventBlockingBitmask event_blocking_bitmask = EVENT_BLOCKING_NONE);
    ~EventBlockingContext();

    private:
    HeroQuestLevelWindow* _hero_quest_level_window;
    EventBlockingBitmask _previous_event_blocking_bitmask;
    EventBlockingBitmask _event_blocking_bitmask;
};

#endif
