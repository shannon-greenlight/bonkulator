
/*
  Bonkulator
*/

typedef void (*FunctionPointer)();
typedef void (*update_fxn)(void);

#define OUTPUT_0_FXN 0
#define OUTPUT_1_FXN 1
#define OUTPUT_2_FXN 2
#define OUTPUT_3_FXN 3
#define OUTPUT_4_FXN 4
#define OUTPUT_5_FXN 5
#define OUTPUT_6_FXN 6
#define OUTPUT_7_FXN 7
#define SETTINGS_FXN 8
#define NUM_FXNS 9

#include <Wire.h>
// Load Greenface libraries
#include "secrets\arduino_secrets.h"
#include "version_num.h"
#include "is_release.h"
#include "face1.h" // greenface logo art

// interrupt variables
volatile uint32_t Timer0Count = 0;

// variables
String version_num = VERSION_NUM;
boolean key_held_down = false;
int16_t adc0;
int16_t adc1;
String wifi_ui_message = "";
int wifi_RSSI;
boolean in_wifi();
uint16_t calculated_time_inc = 0;

// fxns defined in this file
void init_all();
void select_fxn(int fxn_index);

// settings
bool in_user_waveforms();
bool in_ask_init();
void ask_init();
String settings_get_device_name();
uint16_t settings_get_encoder_type();
int settings_get_inactivity_timeout();
bool wifi_enabled(void);
bool usb_direct_enabled();
void settings_put_usb_direct(int val);
void timer_service_settings();
void init_parameters();

// cv fxns
void cv_set(int cv_num, int output, int16_t cv_val);
int cv_time_inc(int cv, float param);
void update_cv(int cv_num);

// timer
FunctionPointer timer_fxn;
void timer_debug();
uint16_t quantize_value(uint16_t value, uint16_t output_num);

// waveforms
void apply_params_to_waveform(int output);
void dump_waveform(int output_num, bool dump_ref);
void graph_waveform(int output_num);
void set_waveform(int, int);
void set_wifi_message();
int calc_wave_value(int i, int waveform, uint16_t waveform_parts);
void recv_user_waveform(String in_str);

// Output Calibration / Corrections
void output_corrections_init();
bool get_output_calibrated(int);
int get_raw_output_offset_correction(int output);
int get_output_scale_correction(int output);

// Input Calibration / Corrections
void input_corrections_init();
bool get_input_calibrated(int input);
int get_raw_input_offset_correction(int input);
int get_input_scale_correction(int input);

// triggers
String check_output_triggers();
bool check_any_triggers();
void clear_all_triggers();
void trigger_output(byte trig_num, int output_num);
void trigger_report();
void clear_trigger(int output_num, int trig_num);

// interface to modules
#include "EEPROM_fxns.h"
#include "lolevel_fxns.h"
#include "hardware_fxns.h"
#include "Rotary_Encoder_fxns.h"
#include <Greenface_gadget.h>
#include "WIFI_Util.h"

// WiFi, USB
String params_toJSON();
String list_fxns();
String output_get_fs();
String output_get_fs_offset();
void wifi_toJSON();

EEPROM_Int selected_output = EEPROM_Int(0, 7); // the output we are working on
EEPROM_Int remembered_fxn = EEPROM_Int(0, 8);  // 0-7=OutputX, 8=Settings
Greenface_gadget *selected_fxn;

#include "TRIGGER.h"

TRIGGER trig0(0);
TRIGGER trig1(1);
TRIGGER trig2(2);
TRIGGER trig3(3);
TRIGGER trig_cv0(4);
TRIGGER trig_cv1(5);
TRIGGER *selected_trigger;
#define NUM_TRIGGERS 4
TRIGGER *triggers[NUM_TRIGGERS] = {&trig0, &trig1, &trig2, &trig3};
void select_trigger(int int_param); // used by Keyboard fxns

uint8_t selected_fxn_num = 0;

#include "BONK_ui.h"
#include <Greenface_wifi_fxns.h>
#include "waveform_data.h"
#include "BONK_output_fxns.h"
#include "user_waveforms_fxns.h"
#include "input_cal_fxns.h"
#include "output_cal_fxns.h"
#include "settings_fxns.h"
#include "hilevel_fxns.h"

#include "Keyboard_fxns.h"
#include "wifi_ui.h"

// guts of DAC servicing
#include "cv_fxns.h"
#include "timer.h"
#include "waveform_fxns.h"
#include "trigger_fxns.h"

// EEPROM_Int init_test = EEPROM_Int(0, 7); // for testing sensing changes to fram

// EEO must come after last EEPROM var is declared
#define EEO Greenface_EEPROM::eeprom_offset
void init_parameters()
{
  init_all_outputs();
  user_waveforms_init();
  settings_init();

  input_cal_init();
  output_cal_init();

  selected_output.put(0);
  remembered_fxn.put(0);

  // do this last
  selected_output.write_int(EEPROM_INIT_FLAG, EEPROM_INIT_PATTERN);
  selected_output.write_int(EEPROM_OFFSET_FLAG, Greenface_EEPROM::eeprom_offset);
}

void init_all()
{
  input_corrections_init();
  output_corrections_init();
  init_parameters();
  wifi_init();
}

void select_fxn(int fxn_index)
{
  fxn_index = constrain(fxn_index, OUTPUT_0_FXN, SETTINGS_FXN);
  selected_fxn_num = fxn_index;
  if (fxn_index == SETTINGS_FXN)
  {
    int param_num = settings_fxn.param_num;
    selected_fxn = &settings_fxn;
    clear_all_triggers();
    settings_fxn.param_num = param_num;
    timer_fxn = &timer_service_settings;
    // outputs_begin();
  }
  else
  {
    selected_fxn = bonk_outputs[fxn_index];
    selected_output.put(fxn_index);
    timer_fxn = &timer_service_outputs;
  }
  remembered_fxn.put(fxn_index);
  selected_fxn->display();
}

// the setup function runs once when you press reset or power the board

void setup()
{
  Wire.begin();
  SPI.begin();
  Serial.begin(115200);

  while (digitalRead(KEYBOARD_COLUMN_0) && !IS_RELEASE)
  {
  };

  ee_info.begin(false);
  ee_info.xfer();

  output_ranges_begin();

  timer_fxn = &timer_service_outputs;
  selected_output.begin(true);
  selected_output.xfer();
  remembered_fxn.begin(false);
  remembered_fxn.xfer();

  // init_test.begin(false);
  // init_test.xfer();

  // delay as bit before starting or else CRASH!!!
  delay(500);

  hardware_begin();
  timer_begin();
  triggers_begin();
  keyboard_begin();
  settings_begin();
  input_cal_begin();
  output_cal_begin();
  rotary_encoder_begin();
  outputs_begin();
  user_waveforms_begin();

  ui.initDisplay();
  wifi_begin();
  if (!eeprom_is_initialized())
  {
    Serial.println(F("EEPROM is not initialized!"));
    init_all();

    // re-begin eeprom stuff
    settings_begin();
    input_cal_begin();
    output_cal_begin();
    outputs_begin();
    user_waveforms_begin();

    // selected_output.test();
  }

  Serial.println("Howdy!");
  ui.begin(face1);
  settings_put_usb_direct(0);
  select_fxn(remembered_fxn.get());
}

// the loop function runs over and over again forever
void loop()
{
  String status_string;
  static bool toggle = false;
  static unsigned long cnt = 0;
  bool debug = false;
  // static unsigned long now;

  check_keyboard();
  if (debug)
    ui.terminal_debug("Keyboard checked");

  check_serial();
  if (debug)
    ui.terminal_debug("Serial checked");

  if (in_output_fxn())
  {
    // now = millis();
    status_string = check_output_triggers();
    // status_string = "Delta: " + String(millis() - now);
    // status_string = "ADC0: " + String(adc0) + " ADC1: " + String(adc1);
    check_outputs();
    if (debug)
      ui.terminal_debug("Outputs checked");
  }
  else if (in_user_waveform_fxn())
  {
    status_string = check_user_recording();
  }
  else if (in_input_cal_fxn())
  {
    status_string = check_input_cal();
  }

  status_string += check_wifi();

  if (debug)
    ui.terminal_debug("Server checked");

  check_rotary_encoder();
  if (debug)
    ui.terminal_debug("Encoder checked");

  if (!in_settings())
  {
    // terminal status interferes with entering strings
    // this is only done in settings IFAIK
    terminal_print_status(status_string);
  }

  ui.check_inactivity_timer(settings_get_inactivity_timeout());
  if (debug)
    ui.terminal_debug("Inactivity checked");

  if (cnt++ > 1)
  {
    toggle = !toggle;
    int val = toggle ? 200 : 255;
    analogWrite(LEDG, val);
    cnt = 0;
  }
  if (debug)
    ui.terminal_debug("Count checked");
}
