// ensure this library description is only included once
#ifndef EEPROM_String_h
#define EEPROM_String_h

// include types & constants of Wiring core API
#include "Arduino.h"
#include <TerminalVT100.h>
#include <Greenface_EEPROM.h>

// library interface description
class EEPROM_String: public Greenface_EEPROM
{
    // user-accessible "public" interface
public:
    EEPROM_String(uint16_t _size);
    //static Greenface_EEPROM gfram;
    uint16_t length() { return val.length(); }
    void xfer(); // xfer from eeprom to array
    String get();
    void put(String val);
    void append(String val);
    char charAt(int indx);
    void putCharAt(char c, int indx);
    void removeCharAt(int indx);
    //boolean update(int framAddr, uint8_t val);
    //void write(int framAddr, uint8_t val);
    //void dump(int start, int len);

    // library-accessible "private" interface
private:
    String val;
    void store(void);   // store to eeprom
};
#endif
