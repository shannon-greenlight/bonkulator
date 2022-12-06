#include "Arduino.h"
#include <TerminalVT100.h>

#include "TRIGGER.h"
#include <Greenface_gadget.h>
#include "hardware_defs.h"

extern String toJSON(String key, String value);
extern void trigger_output(byte trig_num, int output_num);
extern bool in_output_fxn();

TRIGGER::TRIGGER(int _trig_num)
{
    trig_num = _trig_num;
    blanking_period = 0;
    for (int output_num = 0; output_num < NUM_OUTPUTS; output_num++)
    {
        blanking_periods[output_num] = 0;
        hold_offs[output_num] = 0;
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
    // blanking_period = start_time + hold_off;
    if (in_output_fxn())
    {
        for (int output_num = 0; output_num < NUM_OUTPUTS; output_num++)
        {
            uint16_t hold_off = hold_offs[output_num];
            // unsigned long blanking_period = blanking_periods[output_num];
            if (get_output(output_num) && start_time > blanking_periods[output_num])
            {
                trigger_output(trig_num, output_num);
                blanking_periods[output_num] = start_time + hold_off;
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

void TRIGGER::set_hold_off(int output_num, uint16_t hold_off)
{
    hold_offs[output_num] = hold_off;
    // Serial.println(" Holdoff: " + String(hold_off));
}

bool TRIGGER::get_output(int output_num)
{
    return bitRead(outputs, output_num) == 1;
}
