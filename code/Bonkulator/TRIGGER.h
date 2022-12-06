// Trigger class definitions
#ifndef TRIGGER_h
#define TRIGGER_h

#include "hardware_defs.h"

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
    // void do_cmd(char cmd);
    void set_output(int output_num, bool val);
    void set_hold_off(int output_num, uint16_t hold_off);
    bool get_output(int output_num);
    String params_toJSON();
    String get_state();
    trigger_state state;

private:
    uint32_t outputs;                  // up to 32 outputs. 8 for now
    uint16_t hold_offs[8];             // 8 outputs
    unsigned long blanking_periods[8]; // 8 outputs
    long start_time;
    long blanking_period = 0;
};

#endif
