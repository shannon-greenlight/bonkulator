#include <Wire.h>
#include "MCP3X21.h"  // https://github.com/pilotak/MCP3X21

const uint8_t address = 0x4D;
const uint16_t ref_voltage = 3300;  // in mV

MCP3221 mcp3221(address);

void setup() {
    Serial.begin(115200);

#if defined(ESP8266)
    Wire.begin(SDA, SCL);
    mcp3221.init(&Wire);
#else
    mcp3221.init();
#endif
}

void loop() {
    uint16_t result = mcp3221.read();

    Serial.print(F("ADC: "));
    Serial.print(result);
    Serial.print(F(", mV: "));
    Serial.println(mcp3221.toVoltage(result, ref_voltage));

    delay(1000);
}
