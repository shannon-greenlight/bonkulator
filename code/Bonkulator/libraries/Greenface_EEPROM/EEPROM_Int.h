// ensure this library description is only included once
#ifndef EEPROM_Int_h
#define EEPROM_Int_h

// include types & constants of Wiring core API
#include "Arduino.h"
#include <TerminalVT100.h>
#include <Greenface_EEPROM.h>

// library interface description
// Note: this is for a 16 bit int
class EEPROM_Int : public Greenface_EEPROM
{
    // user-accessible "public" interface
public:
    EEPROM_Int(int16_t _min, int16_t _max);
    EEPROM_Int();
    int16_t max, min;
    void xfer(); // xfer from eeprom to int
    int get();
    void put(int16_t _val);
    void inc(int16_t by_val = 1);
    // boolean update(int val);

    // library-accessible "private" interface
private:
    int16_t val;
    void fit_range(void);
};
#endif
