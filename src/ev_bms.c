#include "ev_bms.h"
#include <stdio.h>

/* ===================== Transition Table ===================== */
typedef struct {
    BmsState current;
    BmsEvent event;
    BmsState next;
} Transition;

static BmsState state;

/* ===================== FSM Table ===================== */
static const Transition transition_table[] = {
    { ST_INIT,        EV_NONE,      ST_IDLE },
    { ST_IDLE,        EV_PLUG_IN,    ST_CHARGING },
    { ST_CHARGING,    EV_PLUG_OUT,   ST_DISCHARGING },
    { ST_CHARGING,    EV_OVERVOLT,   ST_FAULT },
    { ST_DISCHARGING, EV_UNDERVOLT,  ST_FAULT },
    { ST_FAULT,       EV_RECOVER,    ST_IDLE }
};

/* ===================== Init ===================== */
void bms_init(void)
{
    state = ST_INIT;
}

/* ===================== FSM Update ===================== */
void bms_update(BmsEvent event)
{
    for (unsigned i = 0;
         i < sizeof(transition_table) / sizeof(transition_table[0]);
         i++)
    {
        if (transition_table[i].current == state &&
            transition_table[i].event == event)
        {
            state = transition_table[i].next;
            break;
        }
    }
}

/* ===================== FSM Execute ===================== */
void bms_execute(void)
{
    switch (state) {
    case ST_IDLE:
        printf("IDLE\n");
        break;

    case ST_CHARGING:
        printf("CHARGING\n");
        break;

    case ST_DISCHARGING:
        printf("DISCHARGING\n");
        break;

    case ST_FAULT:
        printf("FAULT\n");
        break;

    default:
        break;
    }
}
