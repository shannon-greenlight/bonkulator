#include <Wire.h>
#include "MCP3X21.h"  // https://github.com/pilotak/MCP3X21

const uint8_t address = 0x4D;
const uint16_t ref_voltage = 3300;  // in mV

MCP3021 mcp3021(address);

void setup() {
    Serial.begin(115200);

#if defined(ESP8266)
    Wire.begin(SDA, SCL);
    mcp3021.init(&Wire);
#else
    mcp3021.init();
#endif
}

void loop() {
    uint16_t result = mcp3021.read();

    Serial.print(F("ADC: "));
    Serial.print(result);
    Serial.print(F(", mV: "));
    Serial.println(mcp3021.toVoltage(result, ref_voltage));

    delay(1000);
}
