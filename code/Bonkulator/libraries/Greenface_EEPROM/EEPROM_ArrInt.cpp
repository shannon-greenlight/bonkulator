#include "Arduino.h"
#include <TerminalVT100.h>
#include <Adafruit_FRAM_I2C.h>

#include <Greenface_EEPROM.h>
#include <EEPROM_ArrInt.h>

EEPROM_ArrInt::EEPROM_ArrInt(int16_t *_ptr, uint16_t _size)
{
    ptr = _ptr;
    size = _size;
    // offset = _offset;
    // size = sizeof(*ptr)/sizeof(uint16_t);
    // Serial.println("Init ArrInt Length: ");    // printing here crashes system!!!
    offset = eeprom_offset;
    eeprom_offset += size * sizeof(int16_t);
    indx = 0;
}

void EEPROM_ArrInt::xfer()
{
    // Serial.println("Xfer ArrInt Length:"+ String(sizeof(*ptr)));
    for (uint16_t i = 0; i < size; i++)
    {
        ptr[i] = read_int(offset + (i * 2));
    }
}

int16_t EEPROM_ArrInt::get(uint16_t index)
{
    if (index >= size)
        index = indx;
    return ptr[index];
}

void EEPROM_ArrInt::put(int16_t val, uint16_t index)
{
    if (index >= size)
        index = indx;
    // Serial.println("Put ArrInt Offset:"+ String(offset)+ " Index: " + String(index));
    ptr[index] = val;
    write_int(offset + (index * 2), val);
}

void EEPROM_ArrInt::fill(int16_t val)
{
    for (uint16_t i = 0; i < size; i++)
    {
        put(val, i);
    }
}
