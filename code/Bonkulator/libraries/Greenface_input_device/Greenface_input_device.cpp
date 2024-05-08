#include "Wire.h"
#include "Greenface_input_device.h"

Greenface_input_device::Greenface_input_device()
{
}

void Greenface_input_device::begin()
{
    device.init();
}

uint16_t Greenface_input_device::read()
{
    return device.read();
}

// private stuff.
