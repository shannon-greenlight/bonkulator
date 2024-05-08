
/****************************************************************************************************************************
  MBED_RPi_Pico_TimerInterrupt.h
  For MBED RP2040-based boards such as Nano_RP2040_Connect, RASPBERRY_PI_PICO, ADAFRUIT_FEATHER_RP2040 and GENERIC_RP2040.
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/MBED_RPI_PICO_TimerInterrupt
  Licensed under MIT license

  The RPI_PICO system timer peripheral provides a global microsecond timebase for the system, and generates
  interrupts based on this timebase. It supports the following features:
    • A single 64-bit counter, incrementing once per microsecond
    • This counter can be read from a pair of latching registers, for race-free reads over a 32-bit bus.
    • Four alarms: match on the lower 32 bits of counter, IRQ on match: TIMER_IRQ_0-TIMER_IRQ_3

  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one RPI_PICO timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.

  Based on SimpleTimer - A timer library for Arduino.
  Author: mromani@ottotecnica.com
  Copyright (c) 2010 OTTOTECNICA Italy

  Based on BlynkTimer.h
  Author: Volodymyr Shymanskyy

  Version: 1.1.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      07/06/2021 Initial coding to support MBED RP2040-based boards such as RASPBERRY_PI_PICO. etc.
  1.0.1   K Hoang      22/10/2021 Fix platform in library.json for PIO
  1.1.0   K.Hoang      22/01/2022 Fix `multiple-definitions` linker error.
*****************************************************************************************************************************/

#pragma once

#ifndef MBED_RPI_PICO_TIMERINTERRUPT_H
#define MBED_RPI_PICO_TIMERINTERRUPT_H

#include "MBED_RPi_Pico_TimerInterrupt.hpp"
#include "MBED_RPi_Pico_TimerInterrupt_Impl.h"

#endif    // MBED_RPI_PICO_TIMERINTERRUPT_H

