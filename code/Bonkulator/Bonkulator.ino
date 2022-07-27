
/*
  Bonkulator
*/

typedef void (*FunctionPointer)();
typedef void (*update_fxn)(void);

// #define WAVEFORM_PARTS 128
// #define NUM_OUTPUTS 8

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

// Load Greenface libraries
#include "secrets\arduino_secrets.h"
#include "version_num.h"
#include "face1.h" // greenface logo art

// interrupt variables
volatile uint32_t Timer0Count = 0;
volatile int16_t adc0;

volatile int keypress = 0; // set by several sources
volatile unsigned long select_button_down_time = 0;
volatile boolean select_button_pushed;
volatile int inactivity_timer = 0;
volatile unsigned long inactivity_timer_last_reading;
// volatile uint16_t cv0_val;
// volatile uint16_t cv1_val;

// variables
String in_str = ""; // for serial input
boolean cmd_available = false;
boolean esc_mode = false;
boolean select_button_available;
String wifi_ui_message = "";
boolean in_wifi = false;
String status_string;

void init_all();
void set_selected_fxn(int fxn_index);
bool key_available();

// settings
String settings_get_device_name();
uint16_t settings_get_encoder_type();
int settings_get_inactivity_timeout();
bool wifi_enabled(void);
void timer_service_settings();
bool get_output_calibrated(int);

// inactivity timer
void reset_inactivity_timer();
void restore_display();

// timer
FunctionPointer timer_fxn;
void dump_waveform(int output_num, bool dump_ref);
void apply_params();
void graph_waveform(int output_num);
void set_waveform(int, int);
void update_cv(int cv_num);
void set_wifi_message();
void timer_debug();
void timer_init();
void set_offset_correction(int val);
void set_scale_correction(int val);
void set_output_calibrated(int val);
int get_scale_correction(int output);
// int get_offset_correction();
void get_corrections();

// triggers
String check_output_triggers();
bool check_any_triggers();
void clear_all_triggers();

volatile int last_encoder_val = 0;
volatile int state;
volatile int encoderValue;
volatile int channel;

void noop() {}

// interface to modules
#include "lolevel_fxns.h"
#include "hardware_fxns.h"
#include "BONK_ui.h"
#include "EEPROM_fxns.h"
#include "Rotary_Encoder_fxns.h"
#include "SPANK_fxn.h"
#include "WIFI_Util.h"
EEPROM_Int selected_output = EEPROM_Int(0, 7); // the output we are working on
EEPROM_Int remembered_fxn = EEPROM_Int(0, 8);  // 0-7=OutputX, 8=Settings
SPANK_fxn *selected_fxn;

#include "TRIGGER.h"

TRIGGER trig0(0);
TRIGGER trig1(1);
TRIGGER trig2(2);
TRIGGER trig3(3);
TRIGGER *selected_trigger;
#define NUM_TRIGGERS 4
TRIGGER *triggers[NUM_TRIGGERS] = {&trig0, &trig1, &trig2, &trig3};
void select_trigger(int int_param); // used by Keyboard fxns

uint8_t selected_fxn_num = 0;

#include "wifi_fxns.h"
#include "BONK_output_fxns.h"
#include "user_waveforms_fxns.h"
#include "output_cal_fxns.h"
#include "settings_fxns.h"
#include "hilevel_fxns.h"

#include "Keyboard_fxns.h"
#include "wifi_ui.h"
#include "inactivity_timer.h"

// guts of DAC servicing
#include "timer.h"
#include "trigger_fxns.h"

// EEO must come after last EEPROM var is declared
#define EEO Greenface_EEPROM::eeprom_offset

void init_all()
{
  init_all_outputs();
  user_waveforms_init();
  settings_init();
  output_cal_init();
  timer_init();

  // do this last
  selected_output.write_int(EEPROM_INIT_FLAG, EEPROM_INIT_PATTERN);
  selected_output.write_int(EEPROM_OFFSET_FLAG, Greenface_EEPROM::eeprom_offset);
}

void set_selected_fxn(int fxn_index)
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
  // delay(5000);
  while (digitalRead(KEYBOARD_COLUMN_0))
  {
  };
  timer_fxn = &timer_service_outputs;
  selected_output.begin(true);
  selected_output.xfer();
  remembered_fxn.begin(false);
  remembered_fxn.xfer();
  ee_info.begin(false);
  ee_info.xfer();

  ui.begin(face1);

  // rotary_encoder_begin();
  hardware_begin();

  timer_begin();
  triggers_begin();
  keyboard_begin();
  settings_begin();
  output_cal_begin();
  rotary_encoder_begin();

  if (!eeprom_is_initialized())
  {
    Serial.println(F("EEPROM is not initialized!"));
    init_all();
    selected_output.test();
  }
  Serial.println("Howdy!");
  outputs_begin();
  user_waveforms_begin();

  wifi_begin();
  Serial.println("Whew... made it!");

  set_selected_fxn(remembered_fxn.get());
}

// the loop function runs over and over again forever
void loop()
{
  static bool toggle = false;
  static unsigned long cnt = 0;
  static unsigned long now;

  check_keyboard();
  // ui.terminal_debug("Keyboard checked");

  check_serial();
  // ui.terminal_debug("Serial checked");

  if (in_output_fxn())
  {
    status_string = check_output_triggers();
    timer_loop();
  }
  else if (in_user_waveform_fxn())
  {
    status_string = check_user_recording();
  }
  terminal_print_status();

  do_server();
  // ui.terminal_debug("Server checked");

  check_rotary_encoder();
  // ui.terminal_debug("Encoder checked");

  check_inactivity_timer();
  // ui.terminal_debug("Inactivity checked");

  if (cnt++ > 1)
  {
    toggle = !toggle;
    int val = toggle ? 200 : 255;
    analogWrite(LEDG, val);
    cnt = 0;
  }
  // ui.terminal_debug("Count checked");
}
