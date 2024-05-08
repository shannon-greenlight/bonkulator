# MBED_RPI_PICO_TimerInterrupt Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/MBED_RPI_PICO_TimerInterrupt.svg?)](https://www.ardu-badge.com/MBED_RPI_PICO_TimerInterrupt)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/MBED_RPI_PICO_TimerInterrupt.svg)](https://github.com/khoih-prog/MBED_RPI_PICO_TimerInterrupt/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/MBED_RPI_PICO_TimerInterrupt/blob/main/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/MBED_RPI_PICO_TimerInterrupt.svg)](http://github.com/khoih-prog/MBED_RPI_PICO_TimerInterrupt/issues)

---
---

## Table of Contents

* [Important Change from v1.1.0](#Important-Change-from-v110)
* [Why do we need this MBED_RPI_PICO_TimerInterrupt library](#why-do-we-need-this-mbed_rpi_pico_timerinterrupt-library)
  * [Features](#features)
  * [Why using ISR-based Hardware Timer Interrupt is better](#why-using-isr-based-hardware-timer-interrupt-is-better)
  * [Currently supported Boards](#currently-supported-boards)
  * [Important Notes about ISR](#important-notes-about-isr)
* [Changelog](changelog.md)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)
* [More useful Information](#more-useful-information)
* [Usage](#usage)
  * [1. Using only Hardware Timer directly](#1-using-only-hardware-timer-directly)
    * [1.1 Init Hardware Timer](#11-init-hardware-timer)
    * [1.2 Set Hardware Timer Interval and attach Timer Interrupt Handler function](#12-set-hardware-timer-interval-and-attach-timer-interrupt-handler-function)
    * [1.3 Set Hardware Timer Frequency and attach Timer Interrupt Handler function](#13-set-hardware-timer-frequency-and-attach-timer-interrupt-handler-function)
  * [2. Using 16 ISR_based Timers from 1 Hardware Timer](#2-using-16-isr_based-timers-from-1-hardware-timer)
    * [2.1 Important Note](#21-important-note)
    * [2.2 Init Hardware Timer and ISR-based Timer](#22-init-hardware-timer-and-isr-based-timer)
    * [2.3 Set Hardware Timer Interval and attach Timer Interrupt Handler functions](#23-set-hardware-timer-interval-and-attach-timer-interrupt-handler-functions)
* [Examples](#examples)
  * [  1. Argument_Complex](examples/Argument_Complex)
  * [  2. Argument_None](examples/Argument_None)
  * [  3. Argument_Simple](examples/Argument_Simple)
  * [  4. Change_Interval](examples/Change_Interval).
  * [  5. ISR_16_Timers_Array_Complex](examples/ISR_16_Timers_Array_Complex)
  * [  6. ISR_Timers_Array_Simple](examples/ISR_Timers_Array_Simple)
  * [  7. SwitchDebounce](examples/SwitchDebounce)
  * [  8. TimerInterruptTest](examples/TimerInterruptTest)
  * [  9. **50ms_HWTimer**](examples/50ms_HWTimer) **New**
  * [ 10. **multiFileProject**](examples/multiFileProject) **New**
* [Example ISR_Timers_Array_Simple](#example-isr_timers_array_simple)
* [Debug Terminal Output Samples](#debug-terminal-output-samples)
  * [1. ISR_Timers_Array_Simple on RaspberryPi Pico](#1-isr_timers_array_simple-on-raspberrypi-pico)
  * [2. TimerInterruptTest on RaspberryPi Pico](#2-timerinterrupttest-on-raspberrypi-pico)
  * [3. Change_Interval on RaspberryPi Pico](#3-change_interval-on-raspberrypi-pico)
  * [4. SwitchDebounce on RaspberryPi Pico](#4-switchdebounce-on-raspberrypi-pico)
  * [5. ISR_16_Timers_Array_Complex on RaspberryPi Pico](#5-isr_16_timers_array_complex-on-raspberrypi-pico)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Issues](#issues)
* [TO DO](#to-do)
* [DONE](#done)
* [Contributions and Thanks](#contributions-and-thanks)
* [Contributing](#contributing)
* [License](#license)
* [Copyright](#copyright)

---
---

### Important Change from v1.1.0

Please have a look at [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)

### Why do we need this [MBED_RPI_PICO_TimerInterrupt library](https://github.com/khoih-prog/MBED_RPI_PICO_TimerInterrupt)

## Features

This library enables you to use Interrupt from Hardware Timers on **MBED RP2040-based boards such as Nano_RP2040_Connect, RASPBERRY_PI_PICO**, using [**Arduino-mbed RP2040** core](https://github.com/arduino/ArduinoCore-mbed)

As **Hardware Timers are rare, and very precious assets** of any board, this library now enables you to use up to **16 ISR-based Timers, while consuming only 1 Hardware Timer**. Timers' interval is very long (**ulong millisecs**).

Now with these new **16 ISR-based timers**, the maximum interval is **practically unlimited** (limited only by unsigned long miliseconds) while **the accuracy is nearly perfect** compared to software timers. 

The most important feature is they're ISR-based timers. Therefore, their executions are **not blocked by bad-behaving functions / tasks**. This important feature is absolutely necessary for mission-critical tasks. 

The [**ISR_16_Timers_Array_Complex**](examples/ISR_16_Timers_Array_Complex) example will demonstrate the nearly perfect accuracy compared to software timers by printing the actual elapsed millisecs of each type of timers.

Being ISR-based timers, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet and Blynk services. You can also have many `(up to 16)` timers to use.

This non-being-blocked important feature is absolutely necessary for mission-critical tasks.

You'll see blynkTimer Software is blocked while system is connecting to WiFi / Internet / Blynk, as well as by blocking task 
in loop(), using delay() function as an example. The elapsed time then is very unaccurate

### Why using ISR-based Hardware Timer Interrupt is better

Imagine you have a system with a **mission-critical** function, measuring water level and control the sump pump or doing something much more important. You normally use a software timer to poll, or even place the function in loop(). But what if another function is **blocking** the loop() or setup().

So your function **might not be executed, and the result would be disastrous.**

You'd prefer to have your function called, no matter what happening with other functions (busy loop, bug, etc.).

The correct choice is to use a Hardware Timer with **Interrupt** to call your function.

These hardware timers, using interrupt, still work even if other functions are blocking. Moreover, they are much more **precise** (certainly depending on clock frequency accuracy) than other software timers using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

Functions using normal software timers, relying on loop() and calling millis(), won't work if the loop() or setup() is blocked by certain operation. For example, certain function is blocking while it's connecting to WiFi or some services.

The catch is **your function is now part of an ISR (Interrupt Service Routine), and must be lean / mean, and follow certain rules.** More to read on:

[**HOWTO Attach Interrupt**](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)

---

### Currently supported Boards

1. RP2040-based boards such as **Nano_RP2040_Connect, RASPBERRY_PI_PICO, ADAFRUIT_FEATHER_RP2040 and GENERIC_RP2040**, etc. using [**Arduino-mbed RP2040** core](https://github.com/arduino/ArduinoCore-mbed)

---

### Important Notes about ISR

1. Inside the attached function, **delay() won’t work and the value returned by millis() will not increment.** Serial data received while in the function may be lost. You should declare as **volatile any variables that you modify within the attached function.**

2. Typically global variables are used to pass data between an ISR and the main program. To make sure variables shared between an ISR and the main program are updated correctly, declare them as volatile.

3. For this MBED-based core, **never use Serial.print(ln) inside ISR or the system will hang**.

---
---


## Prerequisites

1. [`Arduino IDE 1.8.19+` for Arduino](https://github.com/arduino/Arduino). [![GitHub release](https://img.shields.io/github/release/arduino/Arduino.svg)](https://github.com/arduino/Arduino/releases/latest)
2. [`Arduino mbed_rp2040 core 2.6.1+`](https://github.com/arduino/ArduinoCore-mbed) for Arduino (Use Arduino Board Manager) RP2040-based boards, such as **Arduino Nano RP2040 Connect, RASPBERRY_PI_PICO, etc.**. [![GitHub release](https://img.shields.io/github/release/arduino/ArduinoCore-mbed.svg)](https://github.com/arduino/ArduinoCore-mbed/releases/latest)
3. To use with certain example
   - [`SimpleTimer library`](https://github.com/jfturcot/SimpleTimer) for [ISR_Timers_Array_Simple](examples/ISR_Timers_Array_Simple) and [ISR_16_Timers_Array_Complex](examples/ISR_16_Timers_Array_Complex) examples.
   
   
---
---

## Installation

### Use Arduino Library Manager

The best and easiest way is to use `Arduino Library Manager`. Search for [**MBED_RPI_PICO_TimerInterrupt**](https://github.com/khoih-prog/MBED_RPI_PICO_TimerInterrupt), then select / install the latest version.
You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/MBED_RPI_PICO_TimerInterrupt.svg?)](https://www.ardu-badge.com/MBED_RPI_PICO_TimerInterrupt) for more detailed instructions.

### Manual Install

Another way to install is to:

1. Navigate to [**MBED_RPI_PICO_TimerInterrupt**](https://github.com/khoih-prog/MBED_RPI_PICO_TimerInterrupt) page.
2. Download the latest release `MBED_RPI_PICO_TimerInterrupt-main.zip`.
3. Extract the zip file to `MBED_RPI_PICO_TimerInterrupt-main` directory 
4. Copy whole `MBED_RPI_PICO_TimerInterrupt-main` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install [**MBED_RPI_PICO_TimerInterrupt** library](https://platformio.org/lib/show/12389/MBED_RPI_PICO_TimerInterrupt) by using [Library Manager](https://platformio.org/lib/show/12389/MBED_RPI_PICO_TimerInterrupt/installation). Search for **MBED_RPI_PICO_TimerInterrupt** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)


---
---


### HOWTO Fix `Multiple Definitions` Linker Error

The current library implementation, using `xyz-Impl.h` instead of standard `xyz.cpp`, possibly creates certain `Multiple Definitions` Linker error in certain use cases.

You can include these `.hpp` files

```
// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "MBED_RPi_Pico_TimerInterrupt.hpp"   //https://github.com/khoih-prog/MBED_RPI_PICO_TimerInterrupt

// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "MBED_RPi_Pico_ISR_Timer.hpp"        //https://github.com/khoih-prog/MBED_RPI_PICO_TimerInterrupt
```

in many files. But be sure to use the following `.h` files **in just 1 `.h`, `.cpp` or `.ino` file**, which must **not be included in any other file**, to avoid `Multiple Definitions` Linker Error

```
// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "MBED_RPi_Pico_TimerInterrupt.h"     //https://github.com/khoih-prog/MBED_RPI_PICO_TimerInterrupt

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "MBED_RPi_Pico_ISR_Timer.h"          //https://github.com/khoih-prog/MBED_RPI_PICO_TimerInterrupt
```

Check the new [**multiFileProject** example](examples/multiFileProject) for a `HOWTO` demo.

---
---

## More useful Information

The RPI_PICO system timer peripheral provides a global microsecond timebase for the system, and generates interrupts based on this timebase. It supports the following features:
  • A single 64-bit counter, incrementing once per microsecond
  • This counter can be read from a pair of latching registers, for race-free reads over a 32-bit bus.
  • Four alarms: match on the lower 32 bits of counter, IRQ on match: **TIMER_IRQ_0-TIMER_IRQ_3**

---

Now with these new `16 ISR-based timers` (while consuming only **1 hardware timer**), the maximum interval is practically unlimited (limited only by unsigned long miliseconds). The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers Therefore, their executions are not blocked by bad-behaving functions / tasks.
This important feature is absolutely necessary for mission-critical tasks. 

The `ISR_Timer_Complex` example will demonstrate the nearly perfect accuracy compared to software timers by printing the actual elapsed millisecs of each type of timers.
Being ISR-based timers, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet and Blynk services. You can also have many `(up to 16)` timers to use.
This non-being-blocked important feature is absolutely necessary for mission-critical tasks. 
You'll see blynkTimer Software is blocked while system is connecting to WiFi / Internet / Blynk, as well as by blocking task in loop(), using delay() function as an example. The elapsed time then is very unaccurate


---
---

## Usage

Before using any Timer, you have to make sure the Timer has not been used by any other purpose.
`TIMER_IRQ_0, TIMER_IRQ_1, TIMER_IRQ_2 and TIMER_IRQ_3` are supported for RP2040-based boards.


### 1. Using only Hardware Timer directly

### 1.1 Init Hardware Timer

```
// Select the timer you're using, from ITimer0(0)-ITimer3(3)
// Init MBED_RPI_PICO_Timer
MBED_RPI_PICO_Timer ITimer1(1);
```

### 1.2 Set Hardware Timer Interval and attach Timer Interrupt Handler function

Use one of these functions with **interval in unsigned long microseconds**

```   
// interval (in us), callback is ISR
bool setInterval(unsigned long interval, pico_timer_callback callback);

// interval (in us), callback is ISR
bool attachInterruptInterval(unsigned long interval, pico_timer_callback callback)
```

as follows

```
// Never use Serial.print inside this mbed ISR. Will hang the system
void TimerHandler(uint alarm_num)
{
  // Doing something here inside ISR
}

#define TIMER_INTERVAL_MS        5000L

// Init MBED_RPI_PICO_Timer
MBED_RPI_PICO_Timer ITimer(0);

void setup()
{
  ....
  
  // Interval in unsigned long microseconds
  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler))
    Serial.println("Starting ITimer OK, millis() = " + String(millis()));
  else
    Serial.println("Can't set ITimer. Select another freq. or timer");
}  
```

### 1.3 Set Hardware Timer Frequency and attach Timer Interrupt Handler function

Use one of these functions with **frequency in float Hz**

```
// frequency (in Hz), callback is ISR
bool setFrequency(float frequency, pico_timer_callback callback)

// frequency (in Hz), callback is ISR
bool attachInterrupt(float frequency, timer_callback callback);
```

as follows

```
// Never use Serial.print inside this mbed ISR. Will hang the system
void TimerHandler(uint alarm_num)
{
  ///////////////////////////////////////////////////////////
  // Always call this for MBED RP2040 before processing ISR
  TIMER_ISR_START(alarm_num);
  ///////////////////////////////////////////////////////////
  
  // Doing something here inside ISR

  ////////////////////////////////////////////////////////////
  // Always call this for MBED RP2040 after processing ISR
  TIMER_ISR_END(alarm_num);
  ////////////////////////////////////////////////////////////
}

#define TIMER_FREQ_HZ        5555.555

// Init MBED_RPI_PICO_Timer
MBED_RPI_PICO_Timer ITimer(0);

void setup()
{
  ....
  
  // Frequency in float Hz
  if (ITimer.attachInterrupt(TIMER_FREQ_HZ, TimerHandler))
    Serial.println("Starting ITimer OK, millis() = " + String(millis()));
  else
    Serial.println("Can't set ITimer. Select another freq. or timer");
}  
```


### 2. Using 16 ISR_based Timers from 1 Hardware Timer

### 2.1 Important Note

The 16 ISR_based Timers, designed for long timer intervals, only support using **unsigned long millisec intervals**. If you have to use much higher frequency or sub-millisecond interval, you have to use the Hardware Timers directly as in [1.3 Set Hardware Timer Frequency and attach Timer Interrupt Handler function](#13-set-hardware-timer-frequency-and-attach-timer-interrupt-handler-function)

### 2.2 Init Hardware Timer and ISR-based Timer

```
// Init MBED_RPI_PICO_Timer
MBED_RPI_PICO_Timer ITimer1(1);

// Init ISR_Timer
// Each ISR_Timer can service 16 different ISR-based timers
MBED_RPI_PICO_ISR_Timer ISR_timer;
```

### 2.3 Set Hardware Timer Interval and attach Timer Interrupt Handler functions

```
// Never use Serial.print inside this mbed ISR. Will hang the system
void TimerHandler(uint alarm_num)
{
  ///////////////////////////////////////////////////////////
  // Always call this for MBED RP2040 before processing ISR
  TIMER_ISR_START(alarm_num);
  ///////////////////////////////////////////////////////////
  
  ISR_timer.run();

  ////////////////////////////////////////////////////////////
  // Always call this for MBED RP2040 after processing ISR
  TIMER_ISR_END(alarm_num);
  ////////////////////////////////////////////////////////////
}

#define HW_TIMER_INTERVAL_MS          50L

#define TIMER_INTERVAL_2S             2000L
#define TIMER_INTERVAL_5S             5000L
#define TIMER_INTERVAL_11S            11000L
#define TIMER_INTERVAL_101S           101000L

// In AVR, avoid doing something fancy in ISR, for example complex Serial.print with String() argument
// The pure simple Serial.prints here are just for demonstration and testing. Must be eliminate in working environment
// Or you can get this run-time error / crash
void doingSomething2s()
{
  // Doing something here inside ISR every 2 seconds
}
  
void doingSomething5s()
{
  // Doing something here inside ISR every 5 seconds
}

void doingSomething11s()
{
  // Doing something here inside ISR  every 11 seconds
}

void doingSomething101s()
{
  // Doing something here inside ISR every 101 seconds
}

void setup()
{
  ....
  
  if (ITimer1.attachInterruptInterval(HW_TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
    Serial.print(F("Starting ITimer1 OK, millis() = ")); Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer1. Select another freq. or timer"));

  // Just to demonstrate, don't use too many ISR Timers if not absolutely necessary
  // You can use up to 16 timer for each ISR_Timer
  ISR_timer.setInterval(TIMER_INTERVAL_2S, doingSomething2s);
  ISR_timer.setInterval(TIMER_INTERVAL_5S, doingSomething5s);
  ISR_timer.setInterval(TIMER_INTERVAL_11S, doingSomething11s);
  ISR_timer.setInterval(TIMER_INTERVAL_101S, doingSomething101s);
}  
```

---
---

### Examples 

 1. [Argument_Complex](examples/Argument_Complex)
 2. [Argument_None](examples/Argument_None)
 3. [Argument_Simple](examples/Argument_Simple) 
 4. [Change_Interval](examples/Change_Interval) 
 5. [ISR_16_Timers_Array_Complex](examples/ISR_16_Timers_Array_Complex)
 6. [ISR_Timers_Array_Simple](examples/ISR_Timers_Array_Simple)
 7. [SwitchDebounce](examples/SwitchDebounce) 
 8. [TimerInterruptTest](examples/TimerInterruptTest)
 9. [**50ms_HWTimer**](examples/50ms_HWTimer) **New**
10. [**multiFileProject**](examples/multiFileProject) **New**

---
---

### Example [ISR_Timers_Array_Simple](examples/ISR_Timers_Array_Simple)

```
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

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "MBED_RPi_Pico_ISR_Timer.h"

#include <SimpleTimer.h>              // https://github.com/schinken/SimpleTimer

// Init MBED_RPI_PICO_Timer
MBED_RPI_PICO_Timer ITimer1(1);

MBED_RPI_PICO_ISRTimer ISR_timer;

#ifndef LED_BUILTIN
  #define LED_BUILTIN       25
#endif

#define LED_TOGGLE_INTERVAL_MS        1000L

// You have to use longer time here if having problem because Arduino AVR clock is low, 16MHz => lower accuracy.
// Tested OK with 1ms when not much load => higher accuracy.
#define TIMER_INTERVAL_MS            1L

volatile uint32_t startMillis = 0;

volatile uint32_t deltaMillis2s = 0;
volatile uint32_t deltaMillis5s = 0;

volatile uint32_t previousMillis2s = 0;
volatile uint32_t previousMillis5s = 0;

// Never use Serial.print inside this mbed ISR. Will hang the system
void TimerHandler(uint alarm_num)
{
  static bool toggle  = false;
  static int timeRun  = 0;

  ///////////////////////////////////////////////////////////
  // Always call this for MBED RP2040 before processing ISR
  TIMER_ISR_START(alarm_num);
  ///////////////////////////////////////////////////////////

  ISR_timer.run();

  // Toggle LED every LED_TOGGLE_INTERVAL_MS = 2000ms = 2s
  if (++timeRun == ((LED_TOGGLE_INTERVAL_MS) / TIMER_INTERVAL_MS) )
  {
    timeRun = 0;

    //timer interrupt toggles pin LED_BUILTIN
    digitalWrite(LED_BUILTIN, toggle);
    toggle = !toggle;
  }

  ////////////////////////////////////////////////////////////
  // Always call this for MBED RP2040 after processing ISR
  TIMER_ISR_END(alarm_num);
  ////////////////////////////////////////////////////////////
}

void doingSomething2s()
{
  unsigned long currentMillis  = millis();

  deltaMillis2s    = currentMillis - previousMillis2s;
  previousMillis2s = currentMillis;
}

void doingSomething5s()
{
  unsigned long currentMillis  = millis();

  deltaMillis5s    = currentMillis - previousMillis5s;
  previousMillis5s = currentMillis;
}

/////////////////////////////////////////////////

#define SIMPLE_TIMER_MS        2000L

// Init SimpleTimer
SimpleTimer simpleTimer;

// Here is software Timer, you can do somewhat fancy stuffs without many issues.
// But always avoid
// 1. Long delay() it just doing nothing and pain-without-gain wasting CPU power.Plan and design your code / strategy ahead
// 2. Very long "do", "while", "for" loops without predetermined exit time.
void simpleTimerDoingSomething2s()
{
  static unsigned long previousMillis = startMillis;

  unsigned long currMillis = millis();

  Serial.print(F("SimpleTimer : programmed ")); Serial.print(SIMPLE_TIMER_MS);
  Serial.print(F("ms, current time ms : ")); Serial.print(currMillis);
  Serial.print(F(", Delta ms : ")); Serial.println(currMillis - previousMillis);

  Serial.print(F("Timer2s actual : ")); Serial.println(deltaMillis2s);
  Serial.print(F("Timer5s actual : ")); Serial.println(deltaMillis5s);
  
  previousMillis = currMillis;
}

////////////////////////////////////////////////

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial);

  Serial.print(F("\nStarting ISR_Timers_Array_Simple on ")); Serial.println(BOARD_NAME);
  Serial.println(MBED_RPI_PICO_TIMER_INTERRUPT_VERSION);

  if (ITimer1.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
    Serial.print(F("Starting ITimer1 OK, millis() = ")); Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer1. Select another freq. or timer"));

  previousMillis5s = previousMillis2s = millis();

  ISR_timer.setInterval(2000L, doingSomething2s);
  ISR_timer.setInterval(5000L, doingSomething5s);

  // You need this timer for non-critical tasks. Avoid abusing ISR if not absolutely necessary.
  simpleTimer.setInterval(SIMPLE_TIMER_MS, simpleTimerDoingSomething2s);
}

#define BLOCKING_TIME_MS      10000L

void loop()
{
  // This unadvised blocking task is used to demonstrate the blocking effects onto the execution and accuracy to Software timer
  // You see the time elapse of ISR_Timer still accurate, whereas very unaccurate for Software Timer
  // The time elapse for 2000ms software timer now becomes 3000ms (BLOCKING_TIME_MS)
  // While that of ISR_Timer is still prefect.
  delay(BLOCKING_TIME_MS);

  // You need this Software timer for non-critical tasks. Avoid abusing ISR if not absolutely necessary
  // You don't need to and never call ISR_Timer.run() here in the loop(). It's already handled by ISR timer.
  simpleTimer.run();
}
```
---
---

### Debug Terminal Output Samples

### 1. ISR_Timers_Array_Simple on RaspberryPi Pico

The following is the sample terminal output when running example [ISR_Timers_Array_Simple](examples/ISR_Timers_Array_Simple) to demonstrate the accuracy of ISR Hardware Timer, **especially when system is very busy**. The ISR timer is **programmed for 2s, is activated exactly after 2.000s !!!**

While software timer, **programmed for 2s, is activated after more than 10.000s !!!**

```
Starting ISR_Timers_Array_Simple on RaspberryPi Pico
MBED_RPi_Pico_TimerInterrupt v1.1.0
[TISR] _timerNo = 1, Clock (Hz) = 1000000.00, _fre (Hz) = 1000.00
[TISR] _count = 0-1000
[TISR] hardware_alarm_set_target, uS = 1000
Starting ITimer1 OK, millis() = 1690
SimpleTimer : programmed 2000ms, current time ms : 11290, Delta ms : 11290
Timer2s actual : 2000
Timer5s actual : 5000
SimpleTimer : programmed 2000ms, current time ms : 20890, Delta ms : 9600
Timer2s actual : 2000
Timer5s actual : 5000
SimpleTimer : programmed 2000ms, current time ms : 30490, Delta ms : 9600
Timer2s actual : 2000
Timer5s actual : 5000
```

---

### 2. TimerInterruptTest on RaspberryPi Pico

The following is the sample terminal output when running example [TimerInterruptTest](examples/TimerInterruptTest) to demonstrate how to start/stop Hardware Timers on MBED RP2040-based boards.

```
Starting TimerInterruptTest on RaspberryPi Pico
MBED_RPi_Pico_TimerInterrupt v1.1.0
[TISR] _timerNo = 0, Clock (Hz) = 1000000.00, _fre (Hz) = 1.00
[TISR] _count = 0-1000000
[TISR] hardware_alarm_set_target, uS = 1000000
Starting ITimer0 OK, millis() = 1787
[TISR] _timerNo = 1, Clock (Hz) = 1000000.00, _fre (Hz) = 0.33
[TISR] _count = 0-3000000
[TISR] hardware_alarm_set_target, uS = 3000000
Starting ITimer1 OK, millis() = 1789
Stop ITimer0, millis() = 5001
Start ITimer0, millis() = 10002
Stop ITimer1, millis() = 15001
Stop ITimer0, millis() = 15003
Start ITimer0, millis() = 20004
Stop ITimer0, millis() = 25005
Start ITimer1, millis() = 30002
Start ITimer0, millis() = 30006
```

---


### 3. Change_Interval on RaspberryPi Pico

The following is the sample terminal output when running example [Change_Interval](examples/Change_Interval) to demonstrate how to change Timer Interval on-the-fly on MBED RP2040-based boards.

```
Starting Change_Interval on RaspberryPi Pico
MBED_RPi_Pico_TimerInterrupt v1.1.0
[TISR] _timerNo = 0, Clock (Hz) = 1000000.00, _fre (Hz) = 0.50
[TISR] _count = 0-2000000
[TISR] hardware_alarm_set_target, uS = 2000000
Starting  ITimer0 OK, millis() = 1282
[TISR] _timerNo = 1, Clock (Hz) = 1000000.00, _fre (Hz) = 0.20
[TISR] _count = 0-5000000
[TISR] hardware_alarm_set_target, uS = 5000000
Starting  ITimer1 OK, millis() = 1284
Time = 10001, Timer0Count = 4, Timer1Count = 1
Time = 20002, Timer0Count = 9, Timer1Count = 3
[TISR] MBED_RPI_PICO_TimerInterrupt: _timerNo = 0 , _fre = 1000000.00
[TISR] _count = 0 - 4000000
[TISR] hardware_alarm_set_target, uS = 4000000
[TISR] MBED_RPI_PICO_TimerInterrupt: _timerNo = 1 , _fre = 1000000.00
[TISR] _count = 0 - 10000000
[TISR] hardware_alarm_set_target, uS = 10000000
Changing Interval, Timer0 = 4000,  Timer1 = 10000
Time = 30003, Timer0Count = 11, Timer1Count = 3
Time = 40004, Timer0Count = 14, Timer1Count = 4
[TISR] MBED_RPI_PICO_TimerInterrupt: _timerNo = 0 , _fre = 1000000.00
[TISR] _count = 0 - 2000000
[TISR] hardware_alarm_set_target, uS = 2000000
[TISR] MBED_RPI_PICO_TimerInterrupt: _timerNo = 1 , _fre = 1000000.00
[TISR] _count = 0 - 5000000
[TISR] hardware_alarm_set_target, uS = 5000000
Changing Interval, Timer0 = 2000,  Timer1 = 5000
Time = 50005, Timer0Count = 18, Timer1Count = 6
```

---

### 4. SwitchDebounce on RaspberryPi Pico

The following is the sample terminal output when running example [SwitchDebounce](examples/SwitchDebounce)

```
Starting SwitchDebounce on RaspberryPi Pico
MBED_RPi_Pico_TimerInterrupt v1.1.0
[TISR] _timerNo = 1, Clock (Hz) = 1000000.00, _fre (Hz) = 1000.00
[TISR] _count = 0-1000
[TISR] hardware_alarm_set_target, uS = 1000
Starting ITimer1 OK, millis() = 1185
SW Pressed, from millis() = 4537
SW Released, from millis() = 4888
SW Pressed total time ms = 351
SW Pressed, from millis() = 5266
SW Released, from millis() = 5604
SW Pressed total time ms = 338
SW Pressed, from millis() = 6600
SW Long Pressed, total time ms = 11589 - 6600 = 4989
SW Long Pressed, total time ms = 11640 - 6600 = 5040
SW Long Pressed, total time ms = 11691 - 6600 = 5091
SW Long Pressed, total time ms = 11742 - 6600 = 5142
SW Long Pressed, total time ms = 11793 - 6600 = 5193
SW Long Pressed, total time ms = 11844 - 6600 = 5244
SW Long Pressed, total time ms = 11895 - 6600 = 5295
SW Released, from millis() = 11996
SW Pressed total time ms = 5396
SW Pressed, from millis() = 13007
SW Released, from millis() = 13155
SW Pressed total time ms = 148
```

---

### 5. ISR_16_Timers_Array_Complex on RaspberryPi Pico

The following is the sample terminal output when running example [ISR_16_Timers_Array_Complex](examples/ISR_16_Timers_Array_Complex) to demonstrate the accuracy of ISR Hardware Timer, **especially when system is very busy**. The SimpleTimer, **programmed for 2s, is activated only after nearly 10s !!!**

```
Starting ISR_16_Timers_Array_Complex on RaspberryPi Pico
MBED_RPi_Pico_TimerInterrupt v1.1.0
[TISR] _timerNo = 0, Clock (Hz) = 1000000.00, _fre (Hz) = 100.00
[TISR] _count = 0-10000
[TISR] hardware_alarm_set_target, uS = 10000
Starting ITimer OK, millis() = 1083
SimpleTimer : 2, ms : 10683, Dms : 9599
Timer : 0, programmed : 5000, actual : 5000
Timer : 1, programmed : 10000, actual : 0
Timer : 2, programmed : 15000, actual : 0
Timer : 3, programmed : 20000, actual : 0
Timer : 4, programmed : 25000, actual : 0
Timer : 5, programmed : 30000, actual : 0
Timer : 6, programmed : 35000, actual : 0
Timer : 7, programmed : 40000, actual : 0
Timer : 8, programmed : 45000, actual : 0
Timer : 9, programmed : 50000, actual : 0
Timer : 10, programmed : 55000, actual : 0
Timer : 11, programmed : 60000, actual : 0
Timer : 12, programmed : 65000, actual : 0
Timer : 13, programmed : 70000, actual : 0
Timer : 14, programmed : 75000, actual : 0
Timer : 15, programmed : 80000, actual : 0
SimpleTimer : 2, ms : 20289, Dms : 9606
Timer : 0, programmed : 5000, actual : 5001
Timer : 1, programmed : 10000, actual : 10000
Timer : 2, programmed : 15000, actual : 15001
Timer : 3, programmed : 20000, actual : 0
Timer : 4, programmed : 25000, actual : 0
Timer : 5, programmed : 30000, actual : 0
Timer : 6, programmed : 35000, actual : 0
Timer : 7, programmed : 40000, actual : 0
Timer : 8, programmed : 45000, actual : 0
Timer : 9, programmed : 50000, actual : 0
Timer : 10, programmed : 55000, actual : 0
Timer : 11, programmed : 60000, actual : 0
Timer : 12, programmed : 65000, actual : 0
Timer : 13, programmed : 70000, actual : 0
Timer : 14, programmed : 75000, actual : 0
Timer : 15, programmed : 80000, actual : 0
SimpleTimer : 2, ms : 29895, Dms : 9606
Timer : 0, programmed : 5000, actual : 5001
Timer : 1, programmed : 10000, actual : 10001
Timer : 2, programmed : 15000, actual : 15001
Timer : 3, programmed : 20000, actual : 20001
Timer : 4, programmed : 25000, actual : 25002
Timer : 5, programmed : 30000, actual : 0
Timer : 6, programmed : 35000, actual : 0
Timer : 7, programmed : 40000, actual : 0
Timer : 8, programmed : 45000, actual : 0
Timer : 9, programmed : 50000, actual : 0
Timer : 10, programmed : 55000, actual : 0
Timer : 11, programmed : 60000, actual : 0
Timer : 12, programmed : 65000, actual : 0
Timer : 13, programmed : 70000, actual : 0
Timer : 14, programmed : 75000, actual : 0
Timer : 15, programmed : 80000, actual : 0
SimpleTimer : 2, ms : 39501, Dms : 9606
Timer : 0, programmed : 5000, actual : 5001
Timer : 1, programmed : 10000, actual : 10001
Timer : 2, programmed : 15000, actual : 15001
Timer : 3, programmed : 20000, actual : 20001
Timer : 4, programmed : 25000, actual : 25002
Timer : 5, programmed : 30000, actual : 30002
Timer : 6, programmed : 35000, actual : 35003
Timer : 7, programmed : 40000, actual : 0
Timer : 8, programmed : 45000, actual : 0
Timer : 9, programmed : 50000, actual : 0
Timer : 10, programmed : 55000, actual : 0
Timer : 11, programmed : 60000, actual : 0
Timer : 12, programmed : 65000, actual : 0
Timer : 13, programmed : 70000, actual : 0
Timer : 14, programmed : 75000, actual : 0
Timer : 15, programmed : 80000, actual : 0
SimpleTimer : 2, ms : 49106, Dms : 9605
Timer : 0, programmed : 5000, actual : 5001
Timer : 1, programmed : 10000, actual : 10001
Timer : 2, programmed : 15000, actual : 15002
Timer : 3, programmed : 20000, actual : 20002
Timer : 4, programmed : 25000, actual : 25002
Timer : 5, programmed : 30000, actual : 30002
Timer : 6, programmed : 35000, actual : 35003
Timer : 7, programmed : 40000, actual : 40003
Timer : 8, programmed : 45000, actual : 45004
Timer : 9, programmed : 50000, actual : 0
Timer : 10, programmed : 55000, actual : 0
Timer : 11, programmed : 60000, actual : 0
Timer : 12, programmed : 65000, actual : 0
Timer : 13, programmed : 70000, actual : 0
Timer : 14, programmed : 75000, actual : 0
Timer : 15, programmed : 80000, actual : 0
SimpleTimer : 2, ms : 58712, Dms : 9606
Timer : 0, programmed : 5000, actual : 5001
Timer : 1, programmed : 10000, actual : 10001
Timer : 2, programmed : 15000, actual : 15002
Timer : 3, programmed : 20000, actual : 20002
Timer : 4, programmed : 25000, actual : 25002
Timer : 5, programmed : 30000, actual : 30002
Timer : 6, programmed : 35000, actual : 35003
Timer : 7, programmed : 40000, actual : 40003
Timer : 8, programmed : 45000, actual : 45004
Timer : 9, programmed : 50000, actual : 50004
Timer : 10, programmed : 55000, actual : 55005
Timer : 11, programmed : 60000, actual : 0
Timer : 12, programmed : 65000, actual : 0
Timer : 13, programmed : 70000, actual : 0
Timer : 14, programmed : 75000, actual : 0
Timer : 15, programmed : 80000, actual : 0
SimpleTimer : 2, ms : 68318, Dms : 9606
Timer : 0, programmed : 5000, actual : 5001
Timer : 1, programmed : 10000, actual : 10001
Timer : 2, programmed : 15000, actual : 15001
Timer : 3, programmed : 20000, actual : 20002
Timer : 4, programmed : 25000, actual : 25002
Timer : 5, programmed : 30000, actual : 30003
Timer : 6, programmed : 35000, actual : 35003
Timer : 7, programmed : 40000, actual : 40003
Timer : 8, programmed : 45000, actual : 45004
Timer : 9, programmed : 50000, actual : 50004
Timer : 10, programmed : 55000, actual : 55005
Timer : 11, programmed : 60000, actual : 60005
Timer : 12, programmed : 65000, actual : 65006
Timer : 13, programmed : 70000, actual : 0
Timer : 14, programmed : 75000, actual : 0
Timer : 15, programmed : 80000, actual : 0
SimpleTimer : 2, ms : 77924, Dms : 9606
Timer : 0, programmed : 5000, actual : 5001
Timer : 1, programmed : 10000, actual : 10001
Timer : 2, programmed : 15000, actual : 15002
Timer : 3, programmed : 20000, actual : 20002
Timer : 4, programmed : 25000, actual : 25003
Timer : 5, programmed : 30000, actual : 30003
Timer : 6, programmed : 35000, actual : 35003
Timer : 7, programmed : 40000, actual : 40003
Timer : 8, programmed : 45000, actual : 45004
Timer : 9, programmed : 50000, actual : 50004
Timer : 10, programmed : 55000, actual : 55005
Timer : 11, programmed : 60000, actual : 60005
Timer : 12, programmed : 65000, actual : 65006
Timer : 13, programmed : 70000, actual : 70006
Timer : 14, programmed : 75000, actual : 75007
Timer : 15, programmed : 80000, actual : 0
SimpleTimer : 2, ms : 87529, Dms : 9605
Timer : 0, programmed : 5000, actual : 5001
Timer : 1, programmed : 10000, actual : 10001
Timer : 2, programmed : 15000, actual : 15002
Timer : 3, programmed : 20000, actual : 20002
Timer : 4, programmed : 25000, actual : 25003
Timer : 5, programmed : 30000, actual : 30003
Timer : 6, programmed : 35000, actual : 35003
Timer : 7, programmed : 40000, actual : 40004
Timer : 8, programmed : 45000, actual : 45004
Timer : 9, programmed : 50000, actual : 50004
Timer : 10, programmed : 55000, actual : 55005
Timer : 11, programmed : 60000, actual : 60005
Timer : 12, programmed : 65000, actual : 65006
Timer : 13, programmed : 70000, actual : 70006
Timer : 14, programmed : 75000, actual : 75007
Timer : 15, programmed : 80000, actual : 80007
```

---
---

### Debug

Debug is enabled by default on Serial.

You can also change the debugging level (_TIMERINTERRUPT_LOGLEVEL_) from 0 to 4

```cpp
// These define's must be placed at the beginning before #include "MBED_RPI_PICO_TimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _TIMERINTERRUPT_LOGLEVEL_     0
```

---

### Troubleshooting

If you get compilation errors, more often than not, you may need to install a newer version of the core for Arduino boards.

Sometimes, the library will only work if you update the board core to the latest version because I am using newly added functions.


---
---

### Issues

Submit issues to: [MBED_RPI_PICO_TimerInterrupt issues](https://github.com/khoih-prog/MBED_RPI_PICO_TimerInterrupt/issues)

---

## TO DO

1. Search for bug and improvement.


---

## DONE

1. Basic hardware timers for **RP2040-based boards such as Nano_RP2040_Connect, RASPBERRY_PI_PICO**, using [**Arduino-mbed RP2040** core](https://github.com/arduino/ArduinoCore-mbed)
2. More hardware-initiated software-enabled timers
3. Longer time interval
4. Add Version String 
5. Add Table of Contents
6. Fix `multiple-definitions` linker error
7. Optimize library code by using `reference-passing` instead of `value-passing`

---
---

### Contributions and Thanks

Many thanks for everyone for bug reporting, new feature suggesting, testing and contributing to the development of this library.


---

## Contributing

If you want to contribute to this project:

- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License

- The library is licensed under [MIT](https://github.com/khoih-prog/MBED_RPI_PICO_TimerInterrupt/blob/main/LICENSE)

---

## Copyright

Copyright 2021- Khoi Hoang


