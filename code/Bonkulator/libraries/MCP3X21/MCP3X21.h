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
#include "Wire.h"
#ifndef MCP3X21_H
#define MCP3X21_H

#define MCP3X21_DEFAULT_ADDRESS 0x4D
// #define MCP3X21_DEFAULT_ADDRESS 0x9B

class MCP3X21
{
public:
  explicit MCP3X21(uint8_t slave_adr);
  ~MCP3X21(void);
  void init(TwoWire *i2c_obj);
  void init();
  uint16_t read();

protected:
  TwoWire *_i2c;
  const uint8_t _address;
};

class MCP3021 : public MCP3X21
{
public:
  explicit MCP3021(uint8_t slave_adr = MCP3X21_DEFAULT_ADDRESS);
  uint16_t toVoltage(uint16_t data, uint32_t vref);
};

class MCP3221 : public MCP3X21
{
public:
  explicit MCP3221(uint8_t slave_adr = MCP3X21_DEFAULT_ADDRESS);
  uint16_t toVoltage(uint16_t data, uint32_t vref);
};

#endif // MCP3X21_H
