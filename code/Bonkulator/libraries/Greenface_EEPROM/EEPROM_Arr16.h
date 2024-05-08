// ensure this library description is only included once
#ifndef EEPROM_Arr16_h
#define EEPROM_Arr16_h

// include types & constants of Wiring core API
#include "Arduino.h"
#include <TerminalVT100.h>
#include <Greenface_EEPROM.h>

// library interface description
class EEPROM_Arr16 : public Greenface_EEPROM
{
    // user-accessible "public" interface
public:
    EEPROM_Arr16(uint16_t *_ptr, uint16_t _size);
    // static Greenface_EEPROM gfram;
    uint16_t *ptr;
    uint16_t indx;
    uint16_t length() { return size; }
    void xfer(); // xfer from eeprom to array
    int get(uint16_t index = -1);
    void put(uint16_t val, uint16_t index = -1);
    void fill(uint16_t val);
    // boolean update(int framAddr, uint8_t val);
    // void write(int framAddr, uint8_t val);
    // void dump(int start, int len);

    // library-accessible "private" interface
private:
};
#endif
