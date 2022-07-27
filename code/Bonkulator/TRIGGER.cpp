#include "Arduino.h"
#include <TerminalVT100.h>

#include "TRIGGER.h"
#include "SPANK_fxn.h"

extern String toJSON(String key, String value);

TRIGGER::TRIGGER(int _trig_num)
{
    trig_num = _trig_num;
}

void TRIGGER::start()
{
    start_time = millis();
}

void TRIGGER::trigger()
{
    start();
    if (start_time > debounce_time)
    {
        state = TRIGGER_ACTIVE;
        debounce_time = start_time + 20;
    }
}

// not used anymore
void TRIGGER::clear()
{
    // outputs = 0;
    Serial.println("Clearing trigger: " + String(trig_num));
}

void TRIGGER::disable_all()
{
    outputs = 0;
    Serial.println("Disabling triggers for TRIG: " + String(trig_num));
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
