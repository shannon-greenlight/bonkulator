
/****************************************************************************************************************************
  MBED_RPi_Pico_TimerInterrupt.hpp
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

#ifndef MBED_RPI_PICO_TIMERINTERRUPT_HPP
#define MBED_RPI_PICO_TIMERINTERRUPT_HPP

#if ( defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_ADAFRUIT_FEATHER_RP2040) || \
      defined(ARDUINO_GENERIC_RP2040) ) && defined(ARDUINO_ARCH_MBED)
  #define USING_MBED_RPI_PICO_TIMER_INTERRUPT        true
#else
  #error This code is intended to run on the MBED RASPBERRY_PI_PICO platform! Please check your Tools->Board setting.
#endif

#ifndef MBED_RPI_PICO_TIMER_INTERRUPT_VERSION
  #define MBED_RPI_PICO_TIMER_INTERRUPT_VERSION       "MBED_RPi_Pico_TimerInterrupt v1.1.0"
  
  #define MBED_RPI_PICO_TIMER_INTERRUPT_VERSION_MAJOR      1
  #define MBED_RPI_PICO_TIMER_INTERRUPT_VERSION_MINOR      1
  #define MBED_RPI_PICO_TIMER_INTERRUPT_VERSION_PATCH      0

  #define MBED_RPI_PICO_TIMER_INTERRUPT_VERSION_INT        1001000  
#endif

#ifndef TIMER_INTERRUPT_DEBUG
  #define TIMER_INTERRUPT_DEBUG      0
#endif

#include "Arduino.h"

#include "TimerInterrupt_Generic_Debug.h"

/*
  To enable an alarm:
  • Enable the interrupt at the timer with a write to the appropriate alarm bit in INTE: i.e. (1 << 0) for ALARM0
  • Enable the appropriate timer interrupt at the processor (see Section 2.3.2)
  • Write the time you would like the interrupt to fire to ALARM0 (i.e. the current value in TIMERAWL plus your desired
    alarm time in microseconds). Writing the time to the ALARM register sets the ARMED bit as a side effect.
  Once the alarm has fired, the ARMED bit will be set to 0 . To clear the latched interrupt, write a 1 to the appropriate bit in
  INTR.
  
  Defined in x.y.z/cores/arduino/mbed/targets/TARGET_RASPBERRYPI/TARGET_RP2040/pico-sdk/rp2_common/hardware_timer/include/hardware/timer.h
  
  typedef void (*hardware_alarm_callback_t)(uint alarm_num);
  
  void hardware_alarm_set_callback(uint alarm_num, hardware_alarm_callback_t callback);
  bool hardware_alarm_set_target(uint alarm_num, absolute_time_t t);  
  void hardware_alarm_cancel(uint alarm_num);
*/

// We can use many timers here
#define MAX_RPI_PICO_NUM_TIMERS      4

static absolute_time_t    absAlarmTime[MAX_RPI_PICO_NUM_TIMERS]; 
static volatile uint64_t  _timerCount [MAX_RPI_PICO_NUM_TIMERS];

void TIMER_ISR_START(uint alarm_num);
void TIMER_ISR_END(uint alarm_num);

class MBED_RPI_PICO_TimerInterrupt;

typedef MBED_RPI_PICO_TimerInterrupt MBED_RPI_PICO_Timer;
   
class MBED_RPI_PICO_TimerInterrupt
{
  private:
   
    uint8_t                       _timerNo;
    hardware_alarm_callback_t     _callback;          // pointer to the local callback function
    float                         _frequency;         // Timer frequency
        
  public:

    MBED_RPI_PICO_TimerInterrupt(uint8_t timerNo)
    {     
      _timerNo = timerNo;
      _callback = NULL;
    };
    
    #define TIM_CLOCK_FREQ      ( (float) 1000000.0f )

    // frequency (in hertz) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be added in the future by adding similar functions here
    bool setFrequency(const float& frequency, hardware_alarm_callback_t callback)
    {
      if (_timerNo < MAX_RPI_PICO_NUM_TIMERS)
      {
        if ( (frequency == 0.0f) || (frequency > 100000.0f) || (callback == NULL) )
        {
          TISR_LOGERROR(F("Error. frequency == 0, higher than 100KHz or callback == NULL "));
        
          return false;
        }
        
        // Hardware timer is preset in RP2040 at 1MHz / 1uS
        _frequency  = frequency;
        _timerCount[_timerNo] = (uint64_t) TIM_CLOCK_FREQ / frequency;
        
        TISR_LOGWARN5(F("_timerNo = "), _timerNo, F(", Clock (Hz) = "), TIM_CLOCK_FREQ, F(", _fre (Hz) = "), _frequency);
        TISR_LOGWARN3(F("_count = "), (uint32_t) (_timerCount[_timerNo] >> 32) , F("-"), (uint32_t) (_timerCount[_timerNo]));
        
        _callback  =  callback;
         
        //void hardware_alarm_set_callback(uint alarm_num, hardware_alarm_callback_t callback);
        //param callback the callback to install, or NULL to unset
        hardware_alarm_set_callback(_timerNo, callback);       
        
        TIMER_ISR_START(_timerNo);
               
        //bool hardware_alarm_set_target(uint alarm_num, absolute_time_t t);
        hardware_alarm_set_target(_timerNo, absAlarmTime[_timerNo]);
         
        TISR_LOGWARN1(F("hardware_alarm_set_target, uS = "), _timerCount[_timerNo]);

        return true;
      }
      else
      {
        TISR_LOGERROR(F("Error. Timer must be 0-3"));
        
        return false;
      }
      
      TIMER_ISR_END(_timerNo);
    }

    // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be added in the future by adding similar functions here
    bool setInterval(const unsigned long& interval, hardware_alarm_callback_t callback)
    {
      return setFrequency((float) (1000000.0f / interval), callback);
    }

    bool attachInterrupt(const float& frequency, hardware_alarm_callback_t callback)
    {
      return setFrequency(frequency, callback);
    }

    // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be added in the future by adding similar functions here
    bool attachInterruptInterval(const unsigned long& interval, hardware_alarm_callback_t callback)
    {
      return setFrequency( (float) ( 1000000.0f / interval), callback);
    }

    void detachInterrupt()
    {
      hardware_alarm_set_callback(_timerNo, NULL);
    }

    void disableTimer()
    {
      hardware_alarm_set_callback(_timerNo, NULL);
    }

    // Duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    void reattachInterrupt()
    {
      TIMER_ISR_START(_timerNo);
      hardware_alarm_set_callback(_timerNo, _callback);
    }

    // Duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    void enableTimer()
    {
      TIMER_ISR_START(_timerNo);
      hardware_alarm_set_callback(_timerNo, _callback);
    }

    // Just stop clock source, clear the count
    void stopTimer()
    {
      hardware_alarm_set_callback(_timerNo, NULL);
    }

    // Just reconnect clock source, start current count from 0
    void restartTimer()
    {
      TIMER_ISR_START(_timerNo);
      hardware_alarm_set_callback(_timerNo, _callback);
    }

    int8_t getTimer() __attribute__((always_inline))
    {
      return _timerNo;
    }
}; // class MBED_RPI_PICO_TimerInterrupt

#endif    // MBED_RPI_PICO_TIMERINTERRUPT_HPP

