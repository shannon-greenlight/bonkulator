#include "Arduino.h"
#include <TerminalVT100.h>

#include "TRIGGER.h"
#include "SPANK_fxn.h"
#include "hardware_defs.h"

extern String toJSON(String key, String value);
extern void trigger_output(byte trig_num, int output_num);
extern bool in_output_fxn();

TRIGGER::TRIGGER(int _trig_num)
{
    trig_num = _trig_num;
    blanking_period = 0;
}

void TRIGGER::start()
{
    start_time = millis();
}

void TRIGGER::trigger()
{
    start();
    if (start_time > blanking_period)
    {
        state = TRIGGER_ACTIVE;
        blanking_period = start_time + 5;
        if (in_output_fxn())
        {
            for (int output_num = 0; output_num < NUM_OUTPUTS; output_num++)
            {
                if (get_output(output_num) == 1)
                {
                    trigger_output(trig_num, output_num);
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
    // Serial.println("Output_num " + String(output_num) + " val: " + String(outputs));
}

bool TRIGGER::get_output(int output_num)
{
    return bitRead(outputs, output_num) == 1;
}
