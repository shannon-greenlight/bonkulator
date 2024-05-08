/****************************************************************************************************************************
  50ms_HWTimer.ino
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
*****************************************************************************************************************************/

/*
   Notes:
   Special design is necessary to share data between interrupt code and the rest of your program.
   Variables usually need to be "volatile" types. Volatile tells the compiler to avoid optimizations that assume
   variable can not spontaneously change. Because your function may change variables while your program is using them,
   the compiler needs this hint. But volatile alone is often not enough.
   When accessing shared variables, usually interrupts must be disabled. Even with volatile,
   if the interrupt changes a multi-byte variable between a sequence of instructions, it can be read incorrectly.
   If your data is multiple variables, such as an array and a count, usually interrupts need to be disabled
   or the entire sequence of your code which accesses the data.
*/

#if ( defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_ADAFRUIT_FEATHER_RP2040) || \
      defined(ARDUINO_GENERIC_RP2040) ) && defined(ARDUINO_ARCH_MBED)
  #define USING_MBED_RPI_PICO_TIMER_INTERRUPT        true
#else
  #error This code is intended to run on the MBED RASPBERRY_PI_PICO platform! Please check your Tools->Board setting.
#endif

// These define's must be placed at the beginning before #include "TimerInterrupt_Generic.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
#define _TIMERINTERRUPT_LOGLEVEL_     4

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "MBED_RPi_Pico_TimerInterrupt.h"

#ifndef LED_BUILTIN
  #define LED_BUILTIN       25        // Pin LED_BUILTIN mapped to pin GPIO25 of RPI_PICO, control on-board LED
#endif

#define PIN_D1              1         // Pin D1 mapped to pin GPIO1 of RPI_PICO

volatile uint32_t Timer0Count = 0;

uint32_t startTime;

// Never use Serial.print inside this mbed ISR. Will hang the system
void TimerHandler0(uint alarm_num)
{ 
  static bool toggle0 = false;

  ///////////////////////////////////////////////////////////
  // Always call this for MBED RP2040 before processing ISR
  TIMER_ISR_START(alarm_num);
  ///////////////////////////////////////////////////////////

  // Flag for checking to be sure ISR is working as Serial.print is not OK here in ISR
  Timer0Count++;

  //timer interrupt toggles pin LED_BUILTIN
  digitalWrite(LED_BUILTIN, toggle0);
  toggle0 = !toggle0;

  ////////////////////////////////////////////////////////////
  // Always call this for MBED RP2040 after processing ISR
  TIMER_ISR_END(alarm_num);
  ////////////////////////////////////////////////////////////
}


void printResult(uint32_t currTime)
{
  static uint32_t lastCount = 0;
  
  Serial.print(F("Time = ")); Serial.print(currTime); 
  Serial.print(F(", Timer0Count = ")); Serial.print(Timer0Count);
  Serial.print(F(", Timer Duration (ms) = ")); Serial.println( (currTime - startTime) / (Timer0Count - lastCount) );

  lastCount = Timer0Count;
}

#define TIMER0_INTERVAL_MS        20

// Init ESP32 timer 0
MBED_RPI_PICO_Timer ITimer0(0);

void setup()
{
  pinMode(LED_BUILTIN,  OUTPUT);
  pinMode(PIN_D1,       OUTPUT);
  
  Serial.begin(115200);
  while (!Serial);

  delay(100);

  Serial.print(F("\nStarting 50ms_HWTimer on ")); Serial.println(BOARD_NAME);
  Serial.println(MBED_RPI_PICO_TIMER_INTERRUPT_VERSION);

  // Interval in microsecs
  if (ITimer0.attachInterruptInterval(TIMER0_INTERVAL_MS * 1000, TimerHandler0))
  {
    Serial.print(F("Starting  ITimer0 OK, millis() = ")); Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer0. Select another freq. or timer"));

  startTime = millis();  
}

#define CHECK_INTERVAL_MS     10000L

void loop()
{
  static uint32_t currTime;

  currTime = millis();

  if (currTime - startTime > CHECK_INTERVAL_MS)
  {
    printResult(currTime);
    startTime = currTime;
  }
}
