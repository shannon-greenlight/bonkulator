#if (defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_ADAFRUIT_FEATHER_RP2040) || \
     defined(ARDUINO_GENERIC_RP2040)) &&                                                                                       \
    defined(ARDUINO_ARCH_MBED)
#define USING_MBED_RPI_PICO_TIMER_INTERRUPT true
#else
#error This code is intended to run on the MBED RASPBERRY_PI_PICO platform! Please check your Tools->Board setting.
#endif

// These define's must be placed at the beginning before #include "TimerInterrupt_Generic.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
#define _TIMERINTERRUPT_LOGLEVEL_ 4

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "MBED_RPi_Pico_TimerInterrupt.h"

volatile uint32_t Timer0Count = 0;

uint32_t startTime;

#define WAVEFORM_PARTS 128
#define NUM_OUTPUTS 8

typedef struct
{
  uint16_t *waveform_data;
  byte data_len;
  byte current_index;
  unsigned long next_time;
  unsigned int time_inc;
  uint16_t output_value;
  uint16_t delay;
  unsigned long debounce_time;
  bool triggered;
  bool repeat;
  bool enable_t0;
  bool enable_t1;
  bool enable_t2;
  bool enable_t3;
} OutputData;

uint16_t waveform0_data[WAVEFORM_PARTS];
uint16_t waveform1_data[WAVEFORM_PARTS];
uint16_t waveform2_data[WAVEFORM_PARTS];
uint16_t waveform3_data[WAVEFORM_PARTS];
uint16_t waveform4_data[WAVEFORM_PARTS];
uint16_t waveform5_data[WAVEFORM_PARTS];
uint16_t waveform6_data[WAVEFORM_PARTS];
uint16_t waveform7_data[WAVEFORM_PARTS];

OutputData outputs[8] =
    {
        // waveform_data, data_len, current_index, next_time, time_inc, output_value, delay, debounce_time, triggered, repeat, enable_t0, enable_t1, enable_t2, enable_t3
        {waveform0_data, WAVEFORM_PARTS, 0, 0, 10, 0, 1000, 0, false, false, true, true, false, false},
        {waveform1_data, WAVEFORM_PARTS, 0, 0, 50, 0, 0, 0, false, false, true, false, true, false},
        {waveform2_data, WAVEFORM_PARTS, 0, 0, 10, 0, 0, 0, false, false, true, true, false, false},
        {waveform3_data, WAVEFORM_PARTS, 0, 0, 25, 0, 1500, 0, false, false, true, false, false, false},
        {waveform4_data, WAVEFORM_PARTS, 0, 0, 1, 0, 0, 0, false, false, true, false, false, false},
        {waveform5_data, WAVEFORM_PARTS, 0, 0, 1, 0, 0, 0, false, false, true, false, false, false},
        {waveform6_data, WAVEFORM_PARTS, 0, 0, 1, 0, 0, 0, false, false, true, false, false, false},
        {waveform7_data, 10, 0, 0, 10, 0, 100, 0, false, true, false, false, false, true},
};

void output_dac(int dac_num, int output_value)
{
  byte me;
  me = (dac_num + 1) << 4 | (output_value >> 4);
  digitalWrite(CS_PIN, LOW);
  shiftOut(DOUT_PIN, CLK_PIN, MSBFIRST, me);
  me = output_value << 4;
  shiftOut(DOUT_PIN, CLK_PIN, MSBFIRST, me);
  digitalWrite(CS_PIN, HIGH);
}

// Never use Serial.print inside this mbed ISR. Will hang the system
void TimerHandler0(uint alarm_num)
{

  ///////////////////////////////////////////////////////////
  // Always call this for MBED RP2040 before processing ISR
  TIMER_ISR_START(alarm_num);
  ///////////////////////////////////////////////////////////

  // Flag for checking to be sure ISR is working as Serial.print is not OK here in ISR
  Timer0Count++;

  int i;
  unsigned long now = millis();
  for (i = 0; i < NUM_OUTPUTS; i++)
  {
    OutputData *outptr = &outputs[i];
    // digitalWrite(LED_BUILTIN, HIGH);
    if (outptr->triggered)
    {
      if (now >= outptr->next_time)
      {
        outptr->output_value = outptr->waveform_data[outptr->current_index];
        output_dac(i, outptr->output_value);
        if (outptr->current_index++ >= outptr->data_len - 1)
        {
          outptr->current_index = 0;
          outptr->triggered = outptr->repeat;
          outptr->next_time = now + outptr->delay;
        }
        else
        {
          outptr->next_time = now + outptr->time_inc;
        }
      }
    }
    // digitalWrite(LED_BUILTIN, LOW);
  }

  ////////////////////////////////////////////////////////////
  // Always call this for MBED RP2040 after processing ISR
  TIMER_ISR_END(alarm_num);
  ////////////////////////////////////////////////////////////
}

void trigger(int output_num, int trig_num)
{
  // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  OutputData *outptr = &outputs[output_num];
  bool triggered = false;
  int led_pin;
  switch (trig_num)
  {
  case 0:
    triggered = outptr->enable_t0;
    if (triggered)
      led_pin = T0_LED;
    break;
  case 1:
    triggered = outptr->enable_t1;
    if (triggered)
      led_pin = T1_LED;
    break;
  case 2:
    triggered = outptr->enable_t2;
    if (triggered)
      led_pin = T2_LED;
    break;
  case 3:
    triggered = outptr->enable_t3;
    if (triggered)
      led_pin = T3_LED;
    break;
  }
  if (triggered)
  {
    // outputs[output_num].triggered = !outputs[output_num].triggered;
    // outptr->triggered = true;
    outptr->triggered = !outptr->triggered;
    outptr->next_time = outptr->delay + millis();
    outptr->debounce_time = 50 + millis(); // debounce
    digitalWrite(led_pin, outptr->triggered);
  }
}

void triggerx(int trig_num)
{
  for (int output_num = 0; output_num < NUM_OUTPUTS; output_num++)
  {
    OutputData *outptr = &outputs[output_num];
    if (millis() > outptr->debounce_time)
    {
      trigger(output_num, trig_num);
    }
  }
}

void trigger0()
{
  triggerx(0);
}

void trigger1()
{
  triggerx(1);
}

void trigger2()
{
  triggerx(2);
}

void trigger3()
{
  triggerx(3);
}

void printResult(uint32_t currTime)
{
  static uint32_t lastCount = 0;

  Serial.print(F("Time = "));
  Serial.print(currTime);
  Serial.print(F(", Timer0Count = "));
  Serial.print(Timer0Count);
  Serial.print(F(", Timer Duration (ms) = "));
  Serial.println((currTime - startTime) / (Timer0Count - lastCount));

  lastCount = Timer0Count;
}

#define CHECK_INTERVAL_MS 10000L
#define TIMER0_INTERVAL_MS 1

// Init ESP32 timer 0
MBED_RPI_PICO_Timer ITimer0(0);

void set_waveforms()
{
  int temp;
  for (int i = 0; i < WAVEFORM_PARTS; i++)
  {
    temp = (sin(6.28 * i / WAVEFORM_PARTS) * 511) + 512;
    waveform0_data[i] = temp;
    // Serial.println(temp);
  }
}

void timer_begin()
{
  // digitalWrite(T2_LED, HIGH);
  digitalWrite(CS_PIN, HIGH);
  set_waveforms();
  Serial.print(F("\nStarting 50ms_HWTimer on "));
  Serial.println(BOARD_NAME);
  Serial.println(MBED_RPI_PICO_TIMER_INTERRUPT_VERSION);

  // Interval in microsecs
  if (ITimer0.attachInterruptInterval(TIMER0_INTERVAL_MS * 1000, TimerHandler0))
  {
    Serial.print(F("Starting  ITimer0 OK, millis() = "));
    Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer0. Select another freq. or timer"));

  startTime = millis();
}

void timer_loop()
{
  bool in_use;
  for (int trig = 0; trig < 4; trig++)
  {
    in_use = false;
    for (int output = 0; output < NUM_OUTPUTS; output++)
    {
      OutputData *outptr = &outputs[output];
      bool this_trig_enabled;
      switch (trig)
      {
      case 0:
        this_trig_enabled = outptr->enable_t0;
        break;
      case 1:
        this_trig_enabled = outptr->enable_t1;
        break;
      case 2:
        this_trig_enabled = outptr->enable_t2;
        break;
      case 3:
        this_trig_enabled = outptr->enable_t3;
        break;
      }
      if (this_trig_enabled && outptr->triggered)
      {
        in_use = true;
      }
    }
    if (!in_use)
    {
      // Serial.println("Turning off LED " + String(trig));
      switch (trig)
      {
      case 0:
        digitalWrite(T0_LED, LOW);
        break;
      case 1:
        digitalWrite(T1_LED, LOW);
        break;
      case 2:
        digitalWrite(T2_LED, LOW);
        break;
      case 3:
        digitalWrite(T3_LED, LOW);
        break;
      }
    }
  }
}
