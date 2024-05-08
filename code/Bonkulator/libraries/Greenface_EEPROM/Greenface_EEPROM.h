/*
  Greenface_EEPROM.h - Greenface_EEPROM library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef Greenface_EEPROM_h
#define Greenface_EEPROM_h

// include types & constants of Wiring core API
#include "Arduino.h"
#include <TerminalVT100.h>
#include <Adafruit_FRAM_I2C.h>

// library interface description
class Greenface_EEPROM
{
    // user-accessible "public" interface
public:
    Greenface_EEPROM(void);
    static int eeprom_offset;
    uint16_t size;
    int offset;
    uint16_t begin(boolean inc_ctr);      // overrides fram begin, returns base address 0x00
    uint16_t end(void) { return 0x8000; } //Standards requires this to be the item after the last valid entry. The returned pointer is invalid.
    uint16_t length() { return end() - 1; }
    void dump(int start, int len);
    uint8_t read(int framAddr);
    void write(int framAddr, uint8_t val);
    boolean update(int framAddr, uint8_t val);
    int read_int(int framAddr);
    void write_int(int framAddr, int val);
    boolean update_int(int framAddr, int val);
    void get(int framAddr, uint8_t *ptr, uint16_t num_bytes);
    void put(int framAddr, uint8_t *ptr, uint16_t num_bytes);
    void test(void);
    void init(void);
    Adafruit_FRAM_I2C fram;

    // library-accessible "private" interface
private:
    uint16_t framAddr;
};
#endif
