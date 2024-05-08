#include "Arduino.h"
#include <TerminalVT100.h>
#include <Adafruit_FRAM_I2C.h>

#include <Greenface_EEPROM.h>
#include <EEPROM_Bool.h>

EEPROM_Bool::EEPROM_Bool()
{
    offset = eeprom_offset;
    size = sizeof(boolean);
    eeprom_offset += 2;
    val = false;
}

void EEPROM_Bool::xfer()
{
    val = read(offset) ? true : false;
    // Serial.println("Xfer bool: "+String(offset) + " val: " + String(val));
}

int EEPROM_Bool::get()
{
    return val == true;
}

void EEPROM_Bool::set()
{
    val = true;
    write(offset, val);
}

void EEPROM_Bool::reset()
{
    val = false;
    write(offset, val);
}

void EEPROM_Bool::toggle()
{
    val = !val;
    write(offset, val);
}
