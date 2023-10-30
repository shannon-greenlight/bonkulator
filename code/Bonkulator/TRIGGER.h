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

#define TRIGGER_CTRL_CHANS 10

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
    void set_channel(int channel_num, bool val);
    void set_trig_ctrls(int channel_num, uint8_t trig_ctrl, uint16_t trig_ctrl_val);
    bool get_channel(int channel_num);
    String params_toJSON();
    String get_state();
    trigger_state state; // Only used by check_user_recording(). Todo: resolve this issue

private:
    uint32_t channels;                                  // up to 32 channels. 8 outputs, 2 Inputs
    uint8_t trig_ctrls[TRIGGER_CTRL_CHANS];             // 8 outputs, 2 Inputs
    uint16_t trig_ctrl_vals[TRIGGER_CTRL_CHANS];        // 8 outputs, 2 Inputs
    unsigned long blanking_periods[TRIGGER_CTRL_CHANS]; // 8 outputs, 2 Inputs
    unsigned long counts[TRIGGER_CTRL_CHANS];           // 8 outputs, 2 Inputs
    unsigned long start_time;
    long blanking_period = 0;
};

#endif
