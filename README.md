# EV Battery Management Controller

## Overview
This project implements a simplified EV Battery Management Controller using a
**table-driven finite state machine (FSM)** and a **super-loop architecture**.

The design focuses on clarity, scalability, and deterministic control flow.

## Key Concepts
- Table-driven FSM
- Super-loop execution
- Clear fault handling and recovery
- Easy extensibility for new states/events
- Linear SoC estimation from pack voltage
- Cycle statistics tracking (faults, charges, balances)

## States

| State        | Description                                      |
|--------------|--------------------------------------------------|
| INIT         | Power-on default; transitions to IDLE on EV_NONE |
| IDLE         | Pack connected, no active current flow            |
| PRECHARGE    | Soft-start phase before full charging begins      |
| CHARGING     | Active charge current flowing into the pack       |
| BALANCING    | Cell equalisation after a full-charge cycle       |
| DISCHARGING  | Pack supplying energy to the drivetrain           |
| SLEEP        | Low-power standby; entered from IDLE/DISCHARGING  |
| FAULT        | Protective shutdown; cleared by EV_RECOVER        |

## Events

| Event          | Description                                    |
|----------------|------------------------------------------------|
| EV_NONE        | Startup trigger (INIT → IDLE)                  |
| EV_PLUG_IN     | Charge cable connected                         |
| EV_PLUG_OUT    | Charge cable disconnected                      |
| EV_PRECHARGE_DONE | Pre-charge complete, ready to charge        |
| EV_OVERVOLT    | Pack voltage exceeded BMS_OVERVOLT_MV          |
| EV_UNDERVOLT   | Pack voltage fell below BMS_UNDERVOLT_MV       |
| EV_OVERCURRENT | Current exceeded BMS_OVERCURRENT_MA            |
| EV_OVERTEMP    | Temperature exceeded BMS_OVERTEMP_DC           |
| EV_FULL        | Pack reached BMS_FULL_MV during charging       |
| EV_BALANCE_DONE| Cell balancing complete                        |
| EV_SLEEP       | System idle; enter low-power sleep             |
| EV_WAKE        | Wake-up signal from sleep                      |
| EV_RECOVER     | Fault cleared; return to IDLE                  |

## Thresholds

| Macro               | Default  | Unit        |
|---------------------|----------|-------------|
| BMS_OVERVOLT_MV     | 4200     | mV          |
| BMS_UNDERVOLT_MV    | 2800     | mV          |
| BMS_FULL_MV         | 4150     | mV          |
| BMS_OVERCURRENT_MA  | 30000    | mA          |
| BMS_OVERTEMP_DC     | 600      | deci-°C     |

## Public API

```c
/* Core FSM */
void     bms_init(void);
void     bms_update(BmsEvent event);
void     bms_execute(void);
void     bms_run(BmsEvent event);   /* update + execute in one call */
void     bms_reset(void);           /* send EV_RECOVER              */

/* State queries */
BmsState    bms_get_state(void);
const char *bms_get_state_name(void);
const char *bms_get_event_name(BmsEvent event);
bool        bms_is_fault(void);
bool        bms_is_charging(void);
bool        bms_is_sleeping(void);

/* Sensor data */
void     bms_set_data(const BmsData *data);
BmsData  bms_get_data(void);
void     bms_check_thresholds(void);
uint8_t  bms_soc_estimate(void);

/* Diagnostics */
void     bms_print_status(void);
BmsStats bms_get_stats(void);
void     bms_clear_stats(void);
void     bms_print_stats(void);
```

## Intended Use
- Embedded systems learning
- EV control simulations
- FSM architecture demonstrations

