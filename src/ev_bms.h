#ifndef EV_BMS_H
#define EV_BMS_H

#include <stdbool.h>
#include <stdint.h>

/* ===================== Thresholds ===================== */
#define BMS_OVERVOLT_MV      4200U   /* mV  pack voltage max              */
#define BMS_UNDERVOLT_MV     2800U   /* mV  pack voltage min              */
#define BMS_FULL_MV          4150U   /* mV  considered fully charged      */
#define BMS_OVERCURRENT_MA   30000U  /* mA  continuous current max        */
#define BMS_OVERTEMP_DC      600     /* deci-Celsius max  (60.0 °C x 10) */

/* ===================== Events ===================== */
typedef enum {
    EV_NONE,
    EV_PLUG_IN,
    EV_PLUG_OUT,
    EV_PRECHARGE_DONE,
    EV_OVERVOLT,
    EV_UNDERVOLT,
    EV_OVERCURRENT,
    EV_OVERTEMP,
    EV_FULL,
    EV_BALANCE_DONE,
    EV_SLEEP,
    EV_WAKE,
    EV_RECOVER,
    EV_COUNT
} BmsEvent;

/* ===================== States ===================== */
typedef enum {
    ST_INIT,
    ST_IDLE,
    ST_PRECHARGE,
    ST_CHARGING,
    ST_BALANCING,
    ST_DISCHARGING,
    ST_SLEEP,
    ST_FAULT,
    ST_COUNT
} BmsState;

/* ===================== Sensor Data ===================== */
typedef struct {
    uint32_t voltage_mv;      /* pack voltage   [mV]            */
    uint32_t current_ma;      /* pack current   [mA]            */
    int32_t  temperature_dc;  /* temperature    [deci-Celsius]  */
    uint8_t  soc_percent;     /* state of charge [0-100 %]      */
} BmsData;

/* ===================== Statistics ===================== */
typedef struct {
    uint32_t fault_count;
    uint32_t charge_cycles;
    uint32_t balance_cycles;
} BmsStats;

/* ===================== Public API ===================== */
void            bms_init(void);
void            bms_update(BmsEvent event);
void            bms_execute(void);
void            bms_run(BmsEvent event);
void            bms_reset(void);

BmsState        bms_get_state(void);
const char     *bms_get_state_name(void);
const char     *bms_get_event_name(BmsEvent event);

bool            bms_is_fault(void);
bool            bms_is_charging(void);
bool            bms_is_sleeping(void);

void            bms_set_data(const BmsData *data);
BmsData         bms_get_data(void);
void            bms_check_thresholds(void);
uint8_t         bms_soc_estimate(void);
void            bms_print_status(void);

BmsStats        bms_get_stats(void);
void            bms_clear_stats(void);
void            bms_print_stats(void);

#endif /* EV_BMS_H */
