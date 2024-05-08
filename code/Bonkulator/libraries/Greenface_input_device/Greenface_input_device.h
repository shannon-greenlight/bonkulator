/*
  Greenface_input_device
  Copyright (c) 2022 Shannon Vance.  All right reserved.
  Released under Creative Commons Attribution-ShareAlike 2.5 Generic License
*/

// ensure this library description is only included once
#ifndef Greenface_input_device_h
#define Greenface_input_device_h

// include types & constants of Wiring core API

// library interface description
using namespace std;
template <typename DEVICE>
class Greenface_input_device
{
    // user-accessible "public" interface
public:
    Greenface_input_device();

    void begin();
    uint16_t read();

    // library-accessible "private" interface
private:
    DEVICE device;
};

#endif
