#include "Arduino.h"
#include <TerminalVT100.h>

#include "TRIGGER.h"
#include <Greenface_gadget.h>
#include "hardware_defs.h"

extern String toJSON(String key, String value);
extern void trigger_output(byte trig_num, int output_num);
extern bool in_output_fxn();
extern void code_red(bool);

TRIGGER::TRIGGER(int _trig_num)
{
    trig_num = _trig_num;
    blanking_period = 0;
    for (int output_num = 0; output_num < NUM_OUTPUTS; output_num++)
    {
        blanking_periods[output_num] = 0;
        trig_ctrl_vals[output_num] = 0;
    }
}

void TRIGGER::start()
{
    start_time = millis();
}

void TRIGGER::trigger()
{
    start();
    state = TRIGGER_ACTIVE;
    if (in_output_fxn())
    {
        for (int output_num = 0; output_num < NUM_OUTPUTS; output_num++)
        {
            uint16_t ctrl_val = trig_ctrl_vals[output_num];
            uint8_t trig_ctrl = trig_ctrls[output_num];
            if (get_output(output_num))
            {
                // trigger_output(trig_num, output_num); // for testing
                switch (trig_ctrl)
                {
                case TRIG_HOLDOFF:
                    if (start_time > blanking_periods[output_num])
                    {
                        trigger_output(trig_num, output_num);
                        blanking_periods[output_num] = start_time + ctrl_val;
                    }
                    break;
                case TRIG_SKIP:
                    if (counts[output_num]++ > ctrl_val)
                    {
                        trigger_output(trig_num, output_num);
                        counts[output_num] = 0;
                    }
                    break;
                case TRIG_DENSITY:
                    if (random(0, 100) > 100 - ctrl_val)
                    {
                        trigger_output(trig_num, output_num);
                    }
                    break;
                default:
                    code_red(true);
                    // trigger_output(trig_num, output_num);
                }
            }
        }
    }
}

// used by user waveform recording
void TRIGGER::clear()
{
    state = TRIGGER_IDLE;
    // Serial.println("Clearing trigger: " + String(trig_num));
}

void TRIGGER::disable_all()
{
    outputs = 0;
    // Serial.println("Disabling triggers for TRIG: " + String(trig_num));
}

String TRIGGER::params_toJSON()
{
    String out = "";
    out += "{ ";
    out += toJSON("trig_num", String(trig_num));
    out += ", ";
    out += toJSON("outputs", String(outputs));
    out += " }";
    return out;
}

void TRIGGER::set_output(int output_num, bool val)
{
    if (val)
    {
        bitSet(outputs, output_num);
    }
    else
    {
        bitClear(outputs, output_num);
    }
}

void TRIGGER::set_trig_ctrls(int output_num, uint8_t trig_ctrl, uint16_t ctrl_val)
{
    trig_ctrls[output_num] = trig_ctrl;
    trig_ctrl_vals[output_num] = ctrl_val;
    start();
    blanking_periods[output_num] = start_time + ctrl_val;
    // Serial.println(" set_trig_ctrls: output - " + String(output_num) + " ctrl: " + String(trig_ctrl) + " val: " + String(ctrl_val));
}

bool TRIGGER::get_output(int output_num)
{
    return bitRead(outputs, output_num) == 1;
}
