#include "Arduino.h"
#include <TerminalVT100.h>
#include <Adafruit_FRAM_I2C.h>

#include <Greenface_EEPROM.h>
#include <EEPROM_Arr16.h>

EEPROM_Arr16::EEPROM_Arr16(uint16_t *_ptr, uint16_t _size)
{
    ptr = _ptr;
    size = _size;
    // offset = _offset;
    // size = sizeof(*ptr)/sizeof(uint16_t);
    // Serial.println("Init Arr16 Length: ");    // printing here crashes system!!!
    offset = eeprom_offset;
    eeprom_offset += size * sizeof(uint16_t);
    indx = 0;
}

void EEPROM_Arr16::xfer()
{
    // Serial.println("Xfer Arr16 Length:"+ String(sizeof(*ptr)));
    for (uint16_t i = 0; i < size; i++)
    {
        ptr[i] = read_int(offset + (i * 2));
    }
}

int EEPROM_Arr16::get(uint16_t index)
{
    if (index >= size)
        index = indx;
    return ptr[index];
}

void EEPROM_Arr16::put(uint16_t val, uint16_t index)
{
    if (index >= size)
        index = indx;
    // Serial.println("Put Arr16 Offset:"+ String(offset)+ " Index: " + String(index));
    ptr[index] = val;
    write_int(offset + (index * 2), val);
}

void EEPROM_Arr16::fill(uint16_t val)
{
    for (uint16_t i = 0; i < size; i++)
    {
        put(val, i);
    }
}
