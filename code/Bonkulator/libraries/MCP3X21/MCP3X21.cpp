/*
MIT License

Copyright (c) 2018 Pavel Slama

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <Wire.h>
#include "MCP3X21.h"

MCP3X21::MCP3X21(uint8_t slave_adr):
    _address(slave_adr) {
}

MCP3X21::~MCP3X21(void) {
    _i2c->~TwoWire();
}

void MCP3X21::init(TwoWire * i2c_obj) {
    _i2c = i2c_obj;
}

void MCP3X21::init() {
    _i2c = &Wire;
}

uint16_t MCP3X21::read() {
    _i2c->requestFrom(_address, 2U);

    if (_i2c->available() == 2) {
        return ((_i2c->read() << 6) | (_i2c->read() >> 2));
    }

    return 0xFFFF;
}

MCP3021::MCP3021(uint8_t slave_adr):
    MCP3X21(slave_adr) {
}

uint16_t MCP3021::toVoltage(uint16_t data, uint32_t vref) {
    return (vref * data / 1023);
}

MCP3221::MCP3221(uint8_t slave_adr):
    MCP3X21(slave_adr) {
}

uint16_t MCP3221::toVoltage(uint16_t data, uint32_t vref) {
    return (vref * data / 4095);
}
