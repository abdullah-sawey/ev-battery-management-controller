#include "ev_bms.h"
#include <stdio.h>
#include <string.h>

/* ===================== Transition Table ===================== */
typedef struct {
    BmsState current;
    BmsEvent event;
    BmsState next;
} Transition;

static BmsState state;
static BmsData  bms_data;
static BmsStats bms_stats;

/* ===================== Name Lookup Tables ===================== */
static const char * const state_names[ST_COUNT] = {
    [ST_INIT]        = "INIT",
    [ST_IDLE]        = "IDLE",
    [ST_PRECHARGE]   = "PRECHARGE",
    [ST_CHARGING]    = "CHARGING",
    [ST_BALANCING]   = "BALANCING",
    [ST_DISCHARGING] = "DISCHARGING",
    [ST_SLEEP]       = "SLEEP",
    [ST_FAULT]       = "FAULT"
};

static const char * const event_names[EV_COUNT] = {
    [EV_NONE]           = "NONE",
    [EV_PLUG_IN]        = "PLUG_IN",
    [EV_PLUG_OUT]       = "PLUG_OUT",
    [EV_PRECHARGE_DONE] = "PRECHARGE_DONE",
    [EV_OVERVOLT]       = "OVERVOLT",
    [EV_UNDERVOLT]      = "UNDERVOLT",
    [EV_OVERCURRENT]    = "OVERCURRENT",
    [EV_OVERTEMP]       = "OVERTEMP",
    [EV_FULL]           = "FULL",
    [EV_BALANCE_DONE]   = "BALANCE_DONE",
    [EV_SLEEP]          = "SLEEP",
    [EV_WAKE]           = "WAKE",
    [EV_RECOVER]        = "RECOVER"
};

/* ===================== FSM Table ===================== */
static const Transition transition_table[] = {
    /* --- Init ------------------------------------------------ */
    { ST_INIT,        EV_NONE,           ST_IDLE        },

    /* --- Idle ------------------------------------------------ */
    { ST_IDLE,        EV_PLUG_IN,        ST_PRECHARGE   },
    { ST_IDLE,        EV_SLEEP,          ST_SLEEP       },

    /* --- Pre-charge ------------------------------------------ */
    { ST_PRECHARGE,   EV_PRECHARGE_DONE, ST_CHARGING    },
    { ST_PRECHARGE,   EV_PLUG_OUT,       ST_IDLE        },
    { ST_PRECHARGE,   EV_OVERVOLT,       ST_FAULT       },
    { ST_PRECHARGE,   EV_OVERTEMP,       ST_FAULT       },

    /* --- Charging -------------------------------------------- */
    { ST_CHARGING,    EV_FULL,           ST_BALANCING   },
    { ST_CHARGING,    EV_PLUG_OUT,       ST_DISCHARGING },
    { ST_CHARGING,    EV_OVERVOLT,       ST_FAULT       },
    { ST_CHARGING,    EV_OVERCURRENT,    ST_FAULT       },
    { ST_CHARGING,    EV_OVERTEMP,       ST_FAULT       },

    /* --- Balancing ------------------------------------------- */
    { ST_BALANCING,   EV_BALANCE_DONE,   ST_IDLE        },
    { ST_BALANCING,   EV_PLUG_OUT,       ST_DISCHARGING },
    { ST_BALANCING,   EV_OVERVOLT,       ST_FAULT       },
    { ST_BALANCING,   EV_OVERTEMP,       ST_FAULT       },

    /* --- Discharging ----------------------------------------- */
    { ST_DISCHARGING, EV_UNDERVOLT,      ST_FAULT       },
    { ST_DISCHARGING, EV_OVERCURRENT,    ST_FAULT       },
    { ST_DISCHARGING, EV_OVERTEMP,       ST_FAULT       },
    { ST_DISCHARGING, EV_PLUG_IN,        ST_PRECHARGE   },
    { ST_DISCHARGING, EV_SLEEP,          ST_SLEEP       },

    /* --- Sleep ----------------------------------------------- */
    { ST_SLEEP,       EV_WAKE,           ST_IDLE        },

    /* --- Fault ----------------------------------------------- */
    { ST_FAULT,       EV_RECOVER,        ST_IDLE        }
};

/* ===================== Log Transition ===================== */
static void log_transition(BmsState from, BmsState to, BmsEvent ev)
{
    printf("[BMS] %s -[%s]-> %s\n",
           state_names[from],
           event_names[ev],
           state_names[to]);
}

/* ===================== Update Stats ===================== */
static void update_stats(BmsState from, BmsState to)
{
    if (to == ST_FAULT) {
        bms_stats.fault_count++;
    }
    if (from == ST_CHARGING && (to == ST_DISCHARGING || to == ST_BALANCING)) {
        bms_stats.charge_cycles++;
    }
    if (from == ST_BALANCING && to == ST_IDLE) {
        bms_stats.balance_cycles++;
    }
}

/* ===================== Init ===================== */
void bms_init(void)
{
    state = ST_INIT;
    memset(&bms_data,  0, sizeof(bms_data));
    memset(&bms_stats, 0, sizeof(bms_stats));
}

/* ===================== FSM Update ===================== */
void bms_update(BmsEvent event)
{
    for (unsigned i = 0;
         i < sizeof(transition_table) / sizeof(transition_table[0]);
         i++)
    {
        if (transition_table[i].current == state &&
            transition_table[i].event   == event)
        {
            BmsState next = transition_table[i].next;
            log_transition(state, next, event);
            update_stats(state, next);
            state = next;
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

    case ST_PRECHARGE:
        printf("PRECHARGE\n");
        break;

    case ST_CHARGING:
        printf("CHARGING\n");
        break;

    case ST_BALANCING:
        printf("BALANCING\n");
        break;

    case ST_DISCHARGING:
        printf("DISCHARGING\n");
        break;

    case ST_SLEEP:
        printf("SLEEP\n");
        break;

    case ST_FAULT:
        printf("FAULT\n");
        break;

    default:
        break;
    }
}

/* ===================== Run ===================== */
void bms_run(BmsEvent event)
{
    bms_update(event);
    bms_execute();
}

/* ===================== Get State ===================== */
BmsState bms_get_state(void)
{
    return state;
}

/* ===================== Get State Name ===================== */
const char *bms_get_state_name(void)
{
    if ((unsigned)state < ST_COUNT) {
        return state_names[state];
    }
    return "UNKNOWN";
}

/* ===================== Get Event Name ===================== */
const char *bms_get_event_name(BmsEvent event)
{
    if ((unsigned)event < EV_COUNT) {
        return event_names[event];
    }
    return "UNKNOWN";
}

/* ===================== Is Fault ===================== */
bool bms_is_fault(void)
{
    return state == ST_FAULT;
}

/* ===================== Is Charging ===================== */
bool bms_is_charging(void)
{
    return state == ST_CHARGING  ||
           state == ST_PRECHARGE ||
           state == ST_BALANCING;
}

/* ===================== Is Sleeping ===================== */
bool bms_is_sleeping(void)
{
    return state == ST_SLEEP;
}

/* ===================== Reset ===================== */
void bms_reset(void)
{
    bms_update(EV_RECOVER);
}

/* ===================== Set Data ===================== */
void bms_set_data(const BmsData *data)
{
    if (data == NULL) {
        return;
    }
    bms_data = *data;
}

/* ===================== Get Data ===================== */
BmsData bms_get_data(void)
{
    return bms_data;
}

/* ===================== Check Thresholds ===================== */
void bms_check_thresholds(void)
{
    if (bms_data.voltage_mv > BMS_OVERVOLT_MV) {
        bms_update(EV_OVERVOLT);
        return;
    }

    if (bms_data.voltage_mv < BMS_UNDERVOLT_MV &&
        bms_data.voltage_mv > 0U)
    {
        bms_update(EV_UNDERVOLT);
        return;
    }

    if (bms_data.current_ma > BMS_OVERCURRENT_MA) {
        bms_update(EV_OVERCURRENT);
        return;
    }

    if (bms_data.temperature_dc > BMS_OVERTEMP_DC) {
        bms_update(EV_OVERTEMP);
        return;
    }

    if (state == ST_CHARGING &&
        bms_data.voltage_mv >= BMS_FULL_MV)
    {
        bms_update(EV_FULL);
    }
}

/* ===================== SoC Estimate ===================== */
uint8_t bms_soc_estimate(void)
{
    uint32_t v   = bms_data.voltage_mv;
    uint32_t soc;

    if (v >= BMS_FULL_MV) {
        soc = 100U;
    } else if (v <= BMS_UNDERVOLT_MV) {
        soc = 0U;
    } else {
        soc = ((v - BMS_UNDERVOLT_MV) * 100U) /
              (BMS_FULL_MV - BMS_UNDERVOLT_MV);
    }

    return (uint8_t)soc;
}

/* ===================== Print Status ===================== */
void bms_print_status(void)
{
    printf("--- BMS Status ---\n");
    printf("State      : %s\n",   bms_get_state_name());
    printf("Voltage    : %u mV\n", bms_data.voltage_mv);
    printf("Current    : %u mA\n", bms_data.current_ma);
    printf("Temp       : %d dC\n", bms_data.temperature_dc);
    printf("SoC        : %u %%\n", bms_soc_estimate());
    printf("------------------\n");
}

/* ===================== Get Stats ===================== */
BmsStats bms_get_stats(void)
{
    return bms_stats;
}

/* ===================== Clear Stats ===================== */
void bms_clear_stats(void)
{
    memset(&bms_stats, 0, sizeof(bms_stats));
}

/* ===================== Print Stats ===================== */
void bms_print_stats(void)
{
    printf("--- BMS Statistics ---\n");
    printf("Fault count    : %u\n", bms_stats.fault_count);
    printf("Charge cycles  : %u\n", bms_stats.charge_cycles);
    printf("Balance cycles : %u\n", bms_stats.balance_cycles);
    printf("----------------------\n");
}

