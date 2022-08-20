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
#include "EEPROM_ArrInt.h"

uint32_t startTime;

// Interrupt routines
uint16_t randomize_value(int value, int randomness_factor)
{
  value += random(-randomness_factor, randomness_factor);
  return constrain(value, 0, DAC_FS); // constrain output
}

uint16_t quantize_value(uint16_t value, uint16_t output_num)
{
  if ((*bonk_outputs[output_num]).get_param(OUTPUT_QUANTIZE))
  {
    value &= 0xFFF8;
  }
  return value;
}

void output_next_value(int output_num)
{
  OutputData *outptr = &outputs[output_num];
  outptr->output_value = outptr->waveform_data[outptr->current_index % outptr->data_len];
  apply_cv(output_num, OUTPUT_CV0, outptr);
  apply_cv(output_num, OUTPUT_CV1, outptr);

  outptr->output_value = randomize_value(outptr->output_value, outptr->randomness_factor);
  outptr->output_value = quantize_value(outptr->output_value, output_num);

  dac_out(output_num, outptr->output_value);
}

// this is an interrupt fxn!
// called by TimerHandler
void timer_service_outputs()
{
  unsigned long now = millis();
  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
    OutputData *outptr = &outputs[i];
    int wave_type = (*bonk_outputs[i]).get_param(OUTPUT_WAVEFORM);
    if (wave_type == WAVEFORM_TOGGLE)
    {
      if (now >= outptr->next_time)
      {
        outptr->next_time = now + 9999999; // a long time, next_time set by trigger
        if ((*bonk_outputs[i]).triggered)
        {
          output_next_value(i);
        }
        else
        {
          send_idle_value(i);
        }
      }
    }
    else
    {
      if ((*bonk_outputs[i]).triggered)
      {
        int delay = (*bonk_outputs[i]).get_param(OUTPUT_POST_DELAY);
        uint16_t trigger_external = (*bonk_outputs[i]).get_param(OUTPUT_CLOCK) == 1;
        (*bonk_outputs[i]).triggered = !trigger_external;
        bool ok_togo = trigger_external ? true : now >= outptr->next_time;
        if (ok_togo)
        {
          if (outptr->current_index == outptr->data_len)
          {
            // start of delay period
            send_idle_value(i);
          }
          else
          {
            output_next_value(i);
          }

          // are we done with waveform yet?
          if (outptr->current_index++ >= (outptr->data_len - (delay == 0 ? 1 : 0)))
          {
            if ((*bonk_outputs[i]).get_param(OUTPUT_REPEAT) > 0)
            {
              if (outptr->repeat_count > 0)
                outptr->repeat_count--;
              (*bonk_outputs[i]).triggered = outptr->repeat_count > 0;
            }
            if (outptr->repeat_count == 0 && !trigger_external)
              outptr->ok_set_idle = true;
            outptr->current_index = 0;
            outptr->next_time = now + delay;
          }
          else
          {
            outptr->next_time = now + outptr->time_inc;
          }
        }
      }
      else
      {
        if (outptr->ok_set_idle)
        {
          send_idle_value(i);
          outptr->ok_set_idle = false;
        }
      }
    }
  }
}

void TimerHandler0(uint alarm_num)
{

  ///////////////////////////////////////////////////////////
  // Always call this for MBED RP2040 before processing ISR
  TIMER_ISR_START(alarm_num);
  ///////////////////////////////////////////////////////////

  // Flag for checking to be sure ISR is working as Serial.print is not OK here in ISR
  Timer0Count++;

  (*timer_fxn)();

  ////////////////////////////////////////////////////////////
  // Always call this for MBED RP2040 after processing ISR
  TIMER_ISR_END(alarm_num);
  ////////////////////////////////////////////////////////////
}

#define TIMER0_INTERVAL_MS 1

// Init ESP32 timer 0
MBED_RPI_PICO_Timer ITimer0(0);

void timer_begin()
{
  // digitalWrite(T2_LED, HIGH);
  digitalWrite(CS_PIN, HIGH);
  Serial.print(F("\nStarting 1ms_HWTimer on "));
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

void timer_debug()
{

  set_waveform(1, 0);

  Serial.println();
  Serial.println("DAC FS: " + String(DAC_FS));
  Serial.println();
  Serial.println("Offset Corrections:");
  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
    Serial.print(String(output_offset_corrections.get(i)) + " ");
  }
  Serial.println();
  dump_waveform(1, true);
}
