#include "hardware_defs.h"
#include <Greenface_AD5328.h>
#include <MCP3X21.h>
#include <Adafruit_ADS1X15.h>

// Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
Adafruit_ADS1015 ads; /* Use this for the 12-bit version */

Greenface_AD5328 output_device(CS_PIN, DOUT_PIN, CLK_PIN, 0);
// MCP3021 cv0_device(0x4D);
// MCP3021 cv1_device(0x53);

void hardware_begin()
{
    pinMode(DOUT_PIN, OUTPUT);
    pinMode(CLK_PIN, OUTPUT);
    pinMode(CS_PIN, OUTPUT);
    pinMode(A0, OUTPUT);

    pinMode(T0_LED, OUTPUT);
    pinMode(T1_LED, OUTPUT);
    pinMode(T2_LED, OUTPUT);
    pinMode(T3_LED, OUTPUT);

    // TODO: Input pullup is broken!!!
    // https://forum.arduino.cc/t/interrupt-on-digital-pin-does-not-work-on-nano-rp2040-connect/960900/2
    // pinMode(T0_PIN, INPUT_PULLUP);
    // pinMode(T1_PIN, INPUT_PULLUP);
    // pinMode(T2_PIN, INPUT_PULLUP);
    // pinMode(T3_PIN, INPUT_PULLUP);

    analogWriteResolution(10);
    output_device.begin(AD5328_LDAC_PASSTHRU, AD5328_VDD_BOTH, AD5328_BUFFERED_BOTH, AD5328_GAIN_NONE);
    // cv0_device.init();
    // cv1_device.init();

    if (!ads.begin())
    {
        Serial.println("Failed to initialize ADS.");
        while (1)
            ;
    }
    ads.setGain(GAIN_TWO);
}

void dac_out(byte dac, int val)
{
    val = DAC_FS - val;
    // val = max(0, val);
    val = constrain(val, 0, DAC_FS);
    output_device.dac_out(dac, val);
    // output_device.dac_out(dac, constrain(val, 0, DAC_FS));
}

void led_debug(bool state, uint8_t led_index)
{
    switch (led_index)
    {
    case 0:
        digitalWrite(T0_LED, state);
        break;
    case 1:
        digitalWrite(T1_LED, state);
        break;
    case 2:
        digitalWrite(T2_LED, state);
        break;
    case 3:
        digitalWrite(T3_LED, state);
        break;
    }
}

void set_t0_led(bool state)
{
    digitalWrite(T0_LED, state);
}
