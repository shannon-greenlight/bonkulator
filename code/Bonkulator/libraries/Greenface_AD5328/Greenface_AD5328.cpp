#include "Wire.h"
#include "Greenface_AD5328.h"

Greenface_AD5328::Greenface_AD5328(int _sync_pin, int _din_pin, int _sck_pin, int _ldac_pin)
{
    sync_pin = _sync_pin;
    din_pin = _din_pin;
    sck_pin = _sck_pin;
    ldac_pin = _ldac_pin;
}

void Greenface_AD5328::begin(int ldac_mode, int vdd, int buf, int gain)
{
    uint16_t val = 0xF000; // high bit true = control, FULL RESET device
    send_to_device(val);
    Serial.print("AD5328 Reset: ");
    Serial.println(val, HEX);

    val = 0xA000;
    val |= ldac_mode;
    send_to_device(val);
    Serial.print("AD5328 LDAC: ");
    Serial.println(val, HEX);

    val = 0x8000; // high bit true = control, Gain/Ref cmd
    val |= gain << 4;
    val |= buf << 2;
    val |= vdd;
    send_to_device(val);
    Serial.print("AD5328 Begin Gain/Buf/Vdd: ");
    Serial.println(val, HEX);

    data_reset();
}

void Greenface_AD5328::data_reset()
{
    send_to_device(0xe0);
}

void Greenface_AD5328::factory_reset()
{
    send_to_device(0xf0);
}

void Greenface_AD5328::dac_out(unsigned int dac_num, unsigned int val)
{
    // val &= 0x03ff;
    // val = val << 2;
    val |= dac_num << 12;
    send_to_device(val);
}

// private stuff.
void Greenface_AD5328::send_to_device(uint16_t val)
{
    byte me;
    digitalWrite(sync_pin, LOW);
    me = val >> 8;
    shiftOut(din_pin, sck_pin, MSBFIRST, me);
    me = val & 0xff;
    shiftOut(din_pin, sck_pin, MSBFIRST, me);
    digitalWrite(sync_pin, HIGH);
}
