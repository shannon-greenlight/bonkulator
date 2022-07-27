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

// volatile uint32_t Timer0Count = 0;

uint32_t startTime;

typedef struct
{
  uint16_t *waveform_data;
  byte data_len;
  byte current_index;
  unsigned long next_time;
  uint16_t output_value;
  int16_t randomness_factor; // range: -1023 -> 1023
  float scale;               // range: 0 - 100
  int16_t offset;            // range: -4095 -> 4095
  uint16_t time_inc;         // range: 1ms - 32767ms
  uint16_t repeat_count;
} OutputData;

uint16_t waveform0_ref[WAVEFORM_PARTS];
uint16_t waveform1_ref[WAVEFORM_PARTS];
uint16_t waveform2_ref[WAVEFORM_PARTS];
uint16_t waveform3_ref[WAVEFORM_PARTS];
uint16_t waveform4_ref[WAVEFORM_PARTS];
uint16_t waveform5_ref[WAVEFORM_PARTS];
uint16_t waveform6_ref[WAVEFORM_PARTS];
uint16_t waveform7_ref[WAVEFORM_PARTS];

uint16_t *waveform_reference[8] = {
    waveform0_ref,
    waveform1_ref,
    waveform2_ref,
    waveform3_ref,
    waveform4_ref,
    waveform5_ref,
    waveform6_ref,
    waveform7_ref,
};

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
        // waveform_data, data_len, current_index, next_time, output_value, randomness_factor, scale, offset, time_inc, repeat_count
        {waveform0_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 10, 0},
        {waveform1_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 0, 0},
        {waveform2_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 0, 0},
        {waveform3_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 0, 0},
        {waveform4_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 0, 0},
        {waveform5_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 0, 0},
        {waveform6_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 0, 0},
        {waveform7_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 0, 0},
};

// corrections
int16_t _scale_corrections[NUM_OUTPUTS] = {0, 0, 0, 0, 0, 0, 0, 0};
int16_t _offset_corrections[NUM_OUTPUTS] = {0, 0, 0, 0, 0, 0, 0, 0};

EEPROM_ArrInt scale_corrections(_scale_corrections, NUM_OUTPUTS);
EEPROM_ArrInt offset_corrections(_offset_corrections, NUM_OUTPUTS);
EEPROM_String output_calibrated(NUM_OUTPUTS);

// void output_dac(int dac_num, int output_value)
// {
//   byte me;
//   me = (dac_num + 1) << 4 | (output_value >> 4);
//   digitalWrite(CS_PIN, LOW);
//   shiftOut(DOUT_PIN, CLK_PIN, MSBFIRST, me);
//   me = output_value << 4;
//   shiftOut(DOUT_PIN, CLK_PIN, MSBFIRST, me);
//   digitalWrite(CS_PIN, HIGH);
// }

void set_idle_value(int output_num)
{
  dac_out(output_num, (*bonk_outputs[output_num]).get_param(OUTPUT_DACVAL) + _offset_corrections[output_num]);
}

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

void apply_cv(int output_num, int cv_num, OutputData *outptr)
{
  int cv_type = (*bonk_outputs[output_num]).get_param(cv_num);
  int val;
  int offset_adjust = DAC_FS * (1 - outptr->scale) / 2;

  switch (cv_type)
  {
  case CV_SCALE:
    outptr->output_value *= outptr->scale;
    outptr->output_value += offset_adjust;
    outptr->output_value = constrain(outptr->output_value, 0, DAC_FS);
    break;
  case CV_OFFSET:
    val = outptr->output_value;
    val += outptr->offset;
    val = max(val, 0);
    outptr->output_value = min(val, DAC_FS);
    // outptr->output_value = constrain(outptr->output_value, 0, DAC_FS);
    break;
  }
}

// this is an interrupt fxn!
// called by TimerHandler
void timer_service_outputs()
{
  unsigned long now = millis();
  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
    OutputData *outptr = &outputs[i];
    if ((*bonk_outputs[i]).triggered)
    {
      if (now >= outptr->next_time)
      {
        // set output with current value
        outptr->output_value = outptr->waveform_data[outptr->current_index];
        apply_cv(i, OUTPUT_CV0, outptr);
        apply_cv(i, OUTPUT_CV1, outptr);

        // outptr->output_value += random(-outptr->randomness_factor, outptr->randomness_factor);
        // outptr->output_value = max(0, outptr->output_value); // constrain output
        outptr->output_value = randomize_value(outptr->output_value, outptr->randomness_factor);

        // if ((*bonk_outputs[i]).get_param(OUTPUT_QUANTIZE))
        // {
        //   // temp += 4;
        //   outptr->output_value &= 0xFFF8;
        // }
        outptr->output_value = quantize_value(outptr->output_value, i);

        dac_out(i, outptr->output_value);

        // are we done with waveform yet?
        // outptr->current_index += 3; // for testing
        if (outptr->current_index++ >= outptr->data_len - 1)
        {
          if ((*bonk_outputs[i]).get_param(OUTPUT_REPEAT) > 0)
          {
            if (outptr->repeat_count > 0)
              outptr->repeat_count--;
            (*bonk_outputs[i]).triggered = outptr->repeat_count > 0;
          }
          outptr->current_index = 0;
          outptr->next_time = now + (*bonk_outputs[i]).get_param(OUTPUT_DELAY);
        }
        else
        {
          outptr->next_time = now + outptr->time_inc;
        }
      }
    }
    else
    {
      // not triggered, output idle value
      // check 10 times per second
      if (Timer0Count % 100 == 0 && selected_fxn != &output_cal_fxn)
      {
        set_idle_value(i);
        // dac_out(i, (*bonk_outputs[i]).get_param(OUTPUT_DACVAL) + _offset_corrections[i]);
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

  // unsigned long now = millis();

  (*timer_fxn)();
  // if (in_output_fxn())
  // {
  //   timer_service_outputs();
  // }

  ////////////////////////////////////////////////////////////
  // Always call this for MBED RP2040 after processing ISR
  TIMER_ISR_END(alarm_num);
  ////////////////////////////////////////////////////////////
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

// waveform fxns
void dump_waveform(int output, bool dump_ref)
{
  byte data_len = outputs[output].data_len;
  uint16_t *waveform_data;
  if (dump_ref)
  {
    waveform_data = waveform_reference[output];
  }
  else
  {
    waveform_data = outputs[output].waveform_data;
  }

  Serial.println("");
  if (dump_ref)
  {
    Serial.print("Reference ");
  }
  Serial.println("Wave data for Output: " + String(output));
  for (int i = 0; i < data_len; i++)
  {
    if (i % 8 == 0)
    {
      Serial.println();
    }
    Serial.print(waveform_data[i]);
    Serial.print(" ");
  }
  Serial.println("");
}

// void randomize_waveform(int output, int randomness)
// {
//   uint16_t *waveform_data = outputs[output].waveform_data;
//   byte data_len = outputs[output].data_len;
//   for (int i = 0; i < data_len; i++)
//   {
//   }
// }

void correct_ref_waveform()
{
  int output = selected_output.get();

  // uint16_t *waveform_data = outputs[output].waveform_data;
  uint16_t *waveform_ref = waveform_reference[output];
  byte data_len = outputs[output].data_len;

  float scale_factor = scale_corrections.get(output) / 10000.;
  int offset_factor = offset_corrections.get(output);
  // int offset_adjust = DAC_MID - DAC_MID * scale_factor;

  // Serial.println("Scale: " + String(scale_corrections.get(output)));
  // Serial.println("Scale Factor: " + String(scale_factor));
  // Serial.println("Offset Factor: " + String(offset_factor));
  // Serial.println("Offset Adjust: " + String(offset_adjust));
  // Serial.println("Offset output: " + String(output));
  // Serial.println();
  // return;

  int temp;

  for (int i = 0; i < data_len; i++)
  {
    temp = ((waveform_ref[i] - DAC_MID) * scale_factor) + .5 + DAC_MID;
    temp += offset_factor;
    // temp += offset_adjust;

    temp = constrain(temp, 0, DAC_FS); // constrain output
    // temp = max(0, temp);      // constrain output
    // temp = min(DAC_FS, temp); // constrain output
    waveform_ref[i] = temp;
  }
}

void apply_params()
{
  int output = selected_output.get();
  float scale = (the_output)().get_param(OUTPUT_SCALE);
  int offset = (the_output)().get_param(OUTPUT_OFFSET);
  int randomness = (the_output)().get_param(OUTPUT_RANDOMNESS);
  bool quantize = (the_output)().get_param(OUTPUT_QUANTIZE);
  OutputData *outptr = &outputs[output];

  uint16_t *waveform_data = outputs[output].waveform_data;
  uint16_t *waveform_ref = waveform_reference[output];
  byte data_len = outputs[output].data_len;

  float scale_factor = scale / 100;
  // int offset_factor = offset - (DAC_FS + 1) + offset_corrections.get(output);
  int offset_factor = ((offset + OUTPUT_OFFSET_OFFSET) / 100.0) * (DAC_FS + 1);
  // int offset_factor = (DAC_FS + 1) - offset;
  int offset_adjust = DAC_FS * (100 - scale) / 200;
  float randomness_factor = DAC_FS * scale_factor * randomness / 200;
  outptr->randomness_factor = (int)randomness_factor;

  // ui.terminal_debug("Apply params -- Offset factor: " + String(offset_factor) + " offset: " + String(offset));
  // ui.terminal_debug("Apply params -- Randomness factor: " + String(randomness_factor) + " int: " + String((int)randomness_factor));

  int temp;

  for (int i = 0; i < data_len; i++)
  {
    temp = waveform_ref[i] * scale_factor;
    temp += offset_factor;
    temp += offset_adjust;
    // temp += random(-randomness_factor, randomness_factor);
    // temp = max(0, temp); // constrain output

    // if (quantize)
    // {
    //   // temp += 4;
    //   temp &= 0xFFF8;
    // }

    temp = max(0, temp);      // constrain output
    temp = min(DAC_FS, temp); // constrain output
    waveform_data[i] = temp;
  }
}

int get_offset_correction(int output)
{
  // Serial.println("Get offset correction: for: " + String(output));
  return offset_corrections.get(output) - OUTPUT_CAL_OFFSET_CORRECTION;
}

int get_scale_correction(int output)
{
  // Serial.println("Get scale correction: for: " + String(output));
  return scale_corrections.get(output);
}

bool get_output_calibrated(int output)
{
  // Serial.println("Get output calibrated: for: " + String(output) + " val: " + output_calibrated.get());
  return output_calibrated.charAt(output) == 'Y';
}

void get_corrections()
{
  int output = output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM);
  output_cal_fxn.put_param(get_offset_correction(output), OUTPUT_CAL_OFFSET);
  output_cal_fxn.put_param(get_scale_correction(output), OUTPUT_CAL_SCALE);
  output_cal_fxn.put_param(get_output_calibrated(output) ? 1 : 0, OUTPUT_CAL_STATE);
}

void set_offset_correction(int val)
{
  int output = output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM);
  // Serial.println("Set offset correction: " + String(val) + " for: " + String(output));
  offset_corrections.put(val, output);
  update_waveform();
}

void set_scale_correction(int val)
{
  int output = output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM);
  // Serial.println("Set scale correction: " + String(val) + " for: " + String(output));
  scale_corrections.put(val, output);
  update_waveform();
}

void set_output_calibrated(int val)
{
  int output = output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM);
  // Serial.println("Set output calibrated: " + String(val) + " for: " + String(output));
  output_calibrated.putCharAt(val == 1 ? 'Y' : 'N', output);
}

void set_wifi_message()
{
  // OutputData *outptr;
  uint16_t *waveform_data;
  byte data_len;
  OutputData *outptr = &outputs[selected_output.get()];

  if (in_output_fxn())
  {
    waveform_data = outptr->waveform_data;
    data_len = outptr->data_len;
  }
  else if (in_user_waveform_fxn())
  {
    waveform_data = _user_waveforms_data[get_user_waveform_num()];
    data_len = WAVEFORM_PARTS;
  }
  else
  {
    return;
  }

  int data[WAVEFORM_PARTS];

  int index = 0; // index into source waveform data
  float index_inc = WAVEFORM_PARTS / float(data_len);
  uint16_t value;
  // int randomness_factor = (the_output)().get_param(OUTPUT_RANDOMNESS);

  for (int i = 0; i < WAVEFORM_PARTS; i++)
  {
    if (i > 0)
      wifi_ui_message += ", ";

    value = waveform_data[index];
    value = randomize_value(value, outptr->randomness_factor);
    value = quantize_value(value, selected_output.get());
    wifi_ui_message += String(value);

    if (i + 1 > (index + 1) * index_inc)
    {
      index++;
    }
  }
}

void graph_waveform(int output_num)
{
  OutputData *outptr = &outputs[selected_output.get()];
  uint16_t *input_waveform = outputs[output_num].waveform_data;
  uint16_t waveform_data[outptr->data_len];
  for (int i = 0; i < outptr->data_len; i++)
  {
    waveform_data[i] = input_waveform[i];
    waveform_data[i] = randomize_value(waveform_data[i], outptr->randomness_factor);
    waveform_data[i] = quantize_value(waveform_data[i], selected_output.get());
  }
  // ui.terminal_debug("Graph waveform - len: " + String(outputs[output_num].data_len));
  ui.graph_waveform(waveform_data, outputs[output_num].data_len);
}

uint16_t calculated_time_inc = 0;

uint16_t get_waveform_parts(int output)
{
  // uint16_t period = 268;
  uint16_t period = (*bonk_outputs[output]).get_param(OUTPUT_PERIOD);
  uint16_t leftover = period % WAVEFORM_PARTS;
  float ratio = (float)period / WAVEFORM_PARTS;
  if (leftover == 0)
  {
    calculated_time_inc = outputs[output].time_inc = period / WAVEFORM_PARTS;
    return WAVEFORM_PARTS;
  }
  else if (ratio < 1.0)
  {
    calculated_time_inc = outputs[output].time_inc = 1;
    return period;
  }
  else
  {
    calculated_time_inc = outputs[output].time_inc = (int)ratio * 2;
    return period / outputs[output].time_inc;
  }
}

uint16_t get_time_inc(int output)
{
  return outputs[output].time_inc;
  // return (*bonk_outputs[output]).get_param(OUTPUT_TIME_INC);
}

void set_waveform(int output, int waveform)
{
  // temp fix
  // waveform = min(5, waveform);

  int temp;
  uint16_t *waveform_ref = waveform_reference[output];
  uint16_t *waveform_data = outputs[output].waveform_data;
  uint16_t waveform_parts = get_waveform_parts(output); // also sets time_inc

  int user_wavenum = waveform - NUM_STD_WAVEFORMS;
  EEPROM_Arr16 user_waveform = (*user_waveforms_data[user_wavenum]);

  outputs[output].data_len = waveform_parts;
  float user_inc_factor = (WAVEFORM_PARTS / (float)waveform_parts);

  // ui.terminal_debug("Set Waveform user_inc_factor: " + String(user_inc_factor) + " parts: " + String(waveform_parts) + " userlen: " + String(user_waveform.length()));

  for (int i = 0; i < waveform_parts; i++)
  {
    switch (waveform)
    {
    case WAVEFORM_SINE:
    case WAVEFORM_INV_SINE:
      temp = (sin(6.28319 * (float)i / waveform_parts) * 512) + 512;
      break;
    case WAVEFORM_RAMP_UP:
    case WAVEFORM_RAMP_DN:
      temp = i * (DAC_FS + 1) / waveform_parts;
      break;

    case WAVEFORM_PULSE:
      temp = i < waveform_parts / 2 ? 0 : DAC_FS;
      break;
    case WAVEFORM_INV_PULSE:
      temp = i >= waveform_parts / 2 ? 0 : DAC_FS;
      break;
    default:
      temp = user_waveform.get(user_inc_factor * i);
      break;
    }

    // deal with inverse waveforms
    switch (waveform)
    {
    case WAVEFORM_INV_SINE:
    case WAVEFORM_RAMP_DN:
      temp = DAC_FS - temp;
      break;

    default:
      break;
    }

    waveform_data[i] = waveform_ref[i] = temp;
    // Serial.println(temp);
  }
  // dump_waveform(selected_output.get(), true);
  correct_ref_waveform();
  // dump_waveform(selected_output.get(), true);
  apply_params();
  // graph_waveform(selected_output.get());
}

void timer_begin()
{
  scale_corrections.begin(false);
  scale_corrections.xfer();
  offset_corrections.begin(false);
  offset_corrections.xfer();
  output_calibrated.begin(false);
  output_calibrated.xfer();
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

void update_cv(int cv_num)
{
  // this clears mods that were applied -- so we can start fresh
  OutputData *outptr = &outputs[selected_output.get()];
  byte cv_type = (the_output)().get_param(cv_num);

  // uint16_t param = get_time_inc(selected_output.get());
  // outptr->time_inc = get_time_inc(selected_output.get());

  outptr->offset = 0;
  outptr->scale = 1.0;
  update_waveform();
}

// condition cv from +/- 1650 (3300/2)
int16_t condition_cv(int16_t cv_val)
{
  cv_val *= 1.2412; // ads1015 full scale = 4.096. System FS = 3.3V, multiply by ratio
  cv_val = constrain(cv_val, -2048, 2047);
  cv_val += 2048;
  return ADC_FS - cv_val;
}

// scales a float value by adjusted linear. Range of adj: param/100 -> param*100, cv of ADC_FS_OVER_2 returns param*1
float cv_scale_pow(int cv, float param)
{
  float temp = ((float)cv / ADC_FS_OVER_4) - 2.0;
  return pow(10, temp) * param;
}

// simple scale -- 0 through 1*param
float cv_scale(int cv, float param)
{
  return ((float)cv / (float)ADC_FS) * param;
}

int cv_time_inc(int cv, float param, SPANK_fxn bonk_output)
{
  int val = cv_scale_pow(cv, param);
  return constrain(val, TIME_INC_MIN, TIME_INC_MAX);
}

int cv_period(int cv, float param, SPANK_fxn bonk_output)
{
  int val = cv_scale_pow(cv, param);
  return constrain(val, PERIOD_MIN, PERIOD_MAX);
}

int16_t cv_offset(int16_t cv_val)
{
  return cv_scale(cv_val, 2 * DAC_FS) - DAC_FS;
  // return 2 * (cv_scale(cv_val, DAC_FS) - DAC_FS / 2);
  // return 2 * (cv_val - 2048);
  // return (cv_val - ADC_FS_OVER_2);
  // return (ADC_FS_OVER_2 - cv_val);
}

bool sole_cv(int cv_num, int cv_type, SPANK_fxn bonk_output)
{
  return ((cv_num == OUTPUT_CV0 && bonk_output.get_param(OUTPUT_CV1) != cv_type) || (cv_num == OUTPUT_CV1 && bonk_output.get_param(OUTPUT_CV0) != cv_type));
}

// sets output data with appropriate cv modifier value
void timer_set_cv(int cv_num, int output, int16_t cv_val)
{

  cv_val = condition_cv(cv_val);

  // cv_val *= 1.2412; // ads1015 full scale = 4.096. System FS = 3.3V, multiply by ratio
  // cv_val = constrain(cv_val, -2048, 2047);
  // cv_val += 2048;
  // cv_val = ADC_FS - cv_val;

  OutputData *outptr = &outputs[output];
  SPANK_fxn bonk_output = (*bonk_outputs[output]);
  byte cv_type = bonk_output.get_param(cv_num);

  // int16_t toffset;
  static float temp_offset;
  static float temp_scale;
  static uint16_t temp_time_inc;

  // now set value
  switch (cv_type)
  {
  case CV_SCALE:
    if (sole_cv(cv_num, CV_SCALE, bonk_output))
    {
      outptr->scale = cv_scale(cv_val, 1.0);
    }
    else
    {
      switch (cv_num)
      {
      case OUTPUT_CV0:
        temp_scale = cv_scale(cv_val, 1.0);
        break;
      case OUTPUT_CV1:
        outptr->scale = cv_scale(cv_val, temp_scale);
        break;
      }
    }
    break;
  case CV_OFFSET:
    if (sole_cv(cv_num, CV_OFFSET, bonk_output))
    {
      outptr->offset = cv_offset(cv_val);
      // ui.terminal_debug("CV Val: " + String(cv_val) + " Offset: " + String(outptr->offset));
    }
    else
    {
      switch (cv_num)
      {
      case OUTPUT_CV0:
        temp_offset = cv_offset(cv_val);
        break;
      case OUTPUT_CV1:
        outptr->offset = cv_offset(cv_val) + temp_offset;
        break;
      }
    }
    break;
  case CV_PERIOD:
    uint16_t time_inc = get_time_inc(output);
    if (sole_cv(cv_num, CV_PERIOD, bonk_output))
    {
      // ui.terminal_debug("CV Val: " + String(cv_val) + " Output: " + String(output));
      // ui.terminal_debug("Time inc: " + String(outptr->time_inc) + " Output: " + String(output));
      outptr->time_inc = cv_time_inc(cv_val, (float)calculated_time_inc, bonk_output);
      // time_inc = cv_time_inc(cv_val, (float)time_inc, bonk_output);
      // ui.terminal_debug("Time inc: " + String(outptr->time_inc) + " Local: " + String(time_inc));
    }
    else
    {
      switch (cv_num)
      {
      case OUTPUT_CV0:
        temp_time_inc = cv_time_inc(cv_val, (float)calculated_time_inc, bonk_output);
        break;
      case OUTPUT_CV1:
        outptr->time_inc = cv_time_inc(cv_val, temp_time_inc, bonk_output);
        break;
      }
    }
    break;
    // case CV_TIME_INC:
    //   uint16_t time_inc = get_time_inc(output);
    //   if (sole_cv(cv_num, CV_TIME_INC, bonk_output))
    //   {
    //     outptr->time_inc = cv_time_inc(cv_val, (float)time_inc, bonk_output);
    //     // ui.terminal_debug("Time inc: " + String(outptr->time_inc) + " Output: " + String(output));
    //   }
    //   else
    //   {
    //     switch (cv_num)
    //     {
    //     case OUTPUT_CV0:
    //       temp_time_inc = cv_time_inc(cv_val, (float)time_inc, bonk_output);
    //       break;
    //     case OUTPUT_CV1:
    //       outptr->time_inc = cv_time_inc(cv_val, temp_time_inc, bonk_output);
    //       break;
    //     }
    //   }
    //   break;
  }
  // if (outptr->repeat_count == 0)
  // {
  //   dac_out(output, bonk_output.get_param(OUTPUT_DACVAL));
  //   // ui.terminal_debug("Scale: " + String(outptr->scale));
  //   // ui.terminal_debug("Offset: " + String(outptr->offset));
  // }
}

void timer_loop()
{
  uint16_t cv0_val, cv1_val;
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
        this_trig_enabled = (*bonk_outputs[output]).get_param(OUTPUT_ENABLE_T0);
        break;
      case 1:
        this_trig_enabled = (*bonk_outputs[output]).get_param(OUTPUT_ENABLE_T1);
        break;
      case 2:
        this_trig_enabled = (*bonk_outputs[output]).get_param(OUTPUT_ENABLE_T2);
        break;
      case 3:
        this_trig_enabled = (*bonk_outputs[output]).get_param(OUTPUT_ENABLE_T3);
        break;
      }
      if (this_trig_enabled && (*bonk_outputs[output]).triggered)
      {
        // Serial.println((*bonk_outputs[output]).name + " triggered by: " + String(trig));
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
        reset_outputs(0);
        break;
      case 1:
        digitalWrite(T1_LED, LOW);
        reset_outputs(1);
        break;
      case 2:
        digitalWrite(T2_LED, LOW);
        reset_outputs(2);
        break;
      case 3:
        digitalWrite(T3_LED, LOW);
        reset_outputs(3);
        break;
      }
    }
  }

  adc0 = ads.readADC_Differential_0_3();
  int16_t adc1 = ads.readADC_Differential_1_3();

  for (int cv = 0; cv < 2; cv++)
  {
    for (int output = 0; output < NUM_OUTPUTS; output++)
    {
      timer_set_cv(OUTPUT_CV0, output, adc0);
      timer_set_cv(OUTPUT_CV1, output, adc1);
    }
  }
}

void timer_init()
{
  scale_corrections.fill(IDEAL_SCALE_CORRECTION);
  scale_corrections.init();
  offset_corrections.fill(0);
  offset_corrections.init();
  output_calibrated.init();
  output_calibrated.put("NNNNNNNN");
}

void timer_debug()
{

  // timer_init();
  set_waveform(1, 0);

  int val;
  Serial.println();
  Serial.println("DAC FS: " + String(DAC_FS));
  Serial.println();
  Serial.println("Offset Corrections:");
  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
    Serial.print(String(offset_corrections.get(i)) + " ");
  }
  Serial.println();
  dump_waveform(1, true);
}
