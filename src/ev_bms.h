#ifndef EV_BMS_H
#define EV_BMS_H

#include <stdbool.h>

/* ===================== Events ===================== */
typedef enum {
    EV_NONE,
    EV_PLUG_IN,
    EV_PLUG_OUT,
    EV_OVERVOLT,
    EV_UNDERVOLT,
    EV_RECOVER
} BmsEvent;

/* ===================== States ===================== */
typedef enum {
    ST_INIT,
    ST_IDLE,
    ST_CHARGING,
    ST_DISCHARGING,
    ST_FAULT
} BmsState;

/* ===================== Public API ===================== */
void bms_init(void);
void bms_update(BmsEvent event);
void bms_execute(void);

#endif /* EV_BMS_H */
