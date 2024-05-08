// ensure this library description is only included once
#ifndef EEPROM_Bool_h
#define EEPROM_Bool_h

// include types & constants of Wiring core API
#include "Arduino.h"
#include <TerminalVT100.h>
#include <Greenface_EEPROM.h>

// library interface description
// Note: this is for a 16 bit int
class EEPROM_Bool: public Greenface_EEPROM
{
    // user-accessible "public" interface
public:
    EEPROM_Bool();
    void xfer(); // xfer from eeprom to boolean
    int get();
    void set();
    void reset();
    void toggle();
    //boolean update(int val);

    // library-accessible "private" interface
private:
    boolean val;
};
#endif
