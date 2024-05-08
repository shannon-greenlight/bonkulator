#include "Arduino.h"
#include <TerminalVT100.h>
#include <Adafruit_FRAM_I2C.h>

#include <Greenface_EEPROM.h>
#include <EEPROM_String.h>

EEPROM_String::EEPROM_String(uint16_t _size)
{
    size = _size;
    offset = eeprom_offset;
    eeprom_offset += size;
    val = "";
}

void EEPROM_String::xfer()
{
    if (val == "")
    {
        // Serial.println("Xfer String, offset:" + String(offset) + " size: " + String(size) + " val: " + val);
        int i = 0;
        byte b;
        do
        {
            b = read(offset + i);
            if (b)
                val += char(b);
            i++;
        } while (b && i < size);
    }
    // Serial.println("val: " + val);
    // for(uint16_t i=0; i<size;i++) {
    //     val += char(read(offset+(i)));
    // }
}

String EEPROM_String::get()
{
    return val;
}

void EEPROM_String::put(String s)
{
    val = s;
    store();
}

void EEPROM_String::putCharAt(char c, int indx)
{
    val[indx] = c;
    store();
}

void EEPROM_String::append(String s)
{
    val += s;
    store();
}

char EEPROM_String::charAt(int indx)
{
    return val[indx];
}

void EEPROM_String::removeCharAt(int indx)
{
    return val.remove(indx);
}

void EEPROM_String::store()
{
    for (uint16_t i = 0; i < size; i++)
    {
        write(offset + (i), val[i]);
    }
}
