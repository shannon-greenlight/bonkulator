// Trigger class definitions
#ifndef TRIGGER_h
#define TRIGGER_h

#include "hardware_defs.h"
enum
{
    TRIG_HOLDOFF,
    TRIG_SKIP,
    TRIG_DENSITY
};

typedef enum _trigger_state
{
    TRIGGER_IDLE = 0,
    TRIGGER_ACTIVE = 1,
} trigger_state;

class TRIGGER
{
public:
    TRIGGER(int);
    byte trig_num;
    void start();
    void trigger();
    void clear();
    void disable_all();
    void trigger_report();
    // void do_cmd(char cmd);
    void set_output(int output_num, bool val);
    void set_trig_ctrls(int output_num, uint8_t trig_ctrl, uint16_t trig_ctrl_val);
    bool get_output(int output_num);
    String params_toJSON();
    String get_state();
    trigger_state state;

private:
    uint32_t outputs;                  // up to 32 outputs. 8 for now
    uint8_t trig_ctrls[8];             // 8 outputs
    uint16_t trig_ctrl_vals[8];        // 8 outputs
    unsigned long blanking_periods[8]; // 8 outputs
    unsigned long counts[8];           // 8 outputs
    long start_time;
    long blanking_period = 0;
};

#endif
