#include "Arduino.h"
#include <TerminalVT100.h>
#include <Adafruit_FRAM_I2C.h>

#include <Greenface_EEPROM.h>
#include <EEPROM_Int.h>

EEPROM_Int::EEPROM_Int(int16_t _min, int16_t _max)
{
    size = 2; // sizeof int16_t
    offset = eeprom_offset;
    eeprom_offset += size;
    min = _min;
    max = _max;
}

EEPROM_Int::EEPROM_Int(void)
{
    // do nothing during init declaration
    // must do actual later
    // for use in structs etc...
}

void EEPROM_Int::xfer()
{
    Serial.println("Xfer Int: " + String(offset));
    val = read_int(offset);
    fit_range();
}

int EEPROM_Int::get()
{
    return val;
}

void EEPROM_Int::put(int16_t _val)
{
    val = _val;
    fit_range();
    // val = min(max,_val);
    // val = max(min,val);
    // Serial.println("Write int: "+ String(offset));
    write_int(offset, val);
}

void EEPROM_Int::inc(int16_t by_val)
{
    val += by_val;
    fit_range();
    write_int(offset, val);
}

void EEPROM_Int::fit_range(void)
{
    if (val < min)
        val = max;
    if (val > max)
        val = min;
    // val = min(max,val);
    // val = max(min,val);
}
