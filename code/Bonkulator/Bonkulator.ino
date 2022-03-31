
/*
  Bonkulator
*/

typedef void (*FunctionPointer)();

// Load Greenface libraries
#include "secrets\arduino_secrets.h"
#include "version_num.h"
#include "face1.h" // greenface logo art

// variables
String in_str = "";                      // for serial input
volatile boolean fp_key_pressed = false; // only set by front panel keypress
volatile int keypress = 0;               // set by several sources
boolean cmd_available = false;
boolean remote_adjusting = false;
boolean esc_mode = false;
boolean output_mode = false;
volatile unsigned long select_button_down_time = 0;
volatile boolean select_button_pushed;
boolean select_button_available;
String wifi_ui_message = "";
boolean in_wifi = false;

// settings
String settings_get_device_name();
uint16_t settings_get_encoder_type();
int settings_get_inactivity_timeout();
void settings_display();

// inactivity timer
void reset_inactivity_timer();
void restore_display();

void code_red(bool on)
{
  analogWrite(LEDR, on ? 0 : 255);
  analogWrite(LEDB, 255);
  // analogWrite(LEDG, 255);
}

bool wifi_enabled(void);
void disable_ext_trigger(); // need to figure this out 4 triggers now not mixed with select

#define ROTARY_P_PIN 4
#define ROTARY_A_PIN 5
#define ROTARY_B_PIN 6

#define CS_PIN 7
#define DOUT_PIN 8
#define CLK_PIN 9

// Triggers
#define T0_PIN 10
#define T1_PIN 11
#define T2_PIN 12
#define T3_PIN 13

// Trigger LEDs
#define T0_LED 0
#define T1_LED 1
#define T2_LED 2
#define T3_LED 3

// interface to modules
#include "BONK_ui.h"
#include "EEPROM_fxns.h"
#include "Rotary_Encoder_fxns.h"
#include "WIFI_Util.h"
#include "SPANK_fxn.h"

SPANK_fxn *selected_fxn;

#include "BONK_output_fxns.h"
#include "settings_fxns.h"
#include "hilevel_fxns.h"

#include "Keyboard_fxns.h"
#include "wifi_ui.h"
#include "inactivity_timer.h"

// guts of DAC servicing
#include "timer.h"

// EEO must come after last EEPROM var is declared
#define EEO Greenface_EEPROM::eeprom_offset

void init_all()
{
  init_all_outputs();
  settings_init();

  // do this last
  selected_output.write_int(EEPROM_INIT_FLAG, EEPROM_INIT_PATTERN);
  selected_output.write_int(EEPROM_OFFSET_FLAG, Greenface_EEPROM::eeprom_offset);
}

// the setup function runs once when you press reset or power the board
void setup()
{
  Wire.begin();
  SPI.begin();
  Serial.begin(9600);
  analogWriteResolution(10);
  while (digitalRead(T0_PIN))
  {
  };
  Serial.println("Howdy!");
  selected_output.begin(true);
  selected_output.xfer();

  ui.begin(face1);

  e.numFxns = 0;
  set_encoder(); // sets msb,lsb for two types of encoder

  pinMode(DOUT_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  pinMode(A0, OUTPUT);

  pinMode(T0_LED, OUTPUT);
  pinMode(T1_LED, OUTPUT);
  pinMode(T2_LED, OUTPUT);
  pinMode(T3_LED, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(T0_PIN), trigger0, RISING);
  attachInterrupt(digitalPinToInterrupt(T1_PIN), trigger1, RISING);
  attachInterrupt(digitalPinToInterrupt(T2_PIN), trigger2, RISING);
  attachInterrupt(digitalPinToInterrupt(T3_PIN), trigger3, RISING);

  timer_begin();
  keyboard_begin();
  settings_begin();
  begin_all_outputs();

  if (!eeprom_is_initialized())
  {
    Serial.println(F("EEPROM is not initialized!"));
    init_all();
    selected_output.test();
  }

  wifi_begin();
  // connect if wifi is active
  wifi_attempt_connect(true);

  Serial.println("Whew... made it!");
  wifi_new_fxn();
  // select_wifi_fxn();
  // splash();
  selected_fxn = &settings_fxn;
  selected_fxn->display();
  // Serial.println("Selected output: " + String(selected_output.get()));
  // selected_output.inc();
}

// the loop function runs over and over again forever
void loop()
{
  static bool toggle = false;
  static unsigned long cnt = 0;

  // code_red(esc_mode);

  scan_keyboard();
  if (key_available())
  {
    calc_keypress();
    process_key();
    wait_all_keys_up();
    delay(10);
    reset_keyboard();
  }

  check_serial();
  if (keypress || cmd_available)
  {
    if (cmd_available)
    {
      process_cmd(in_str);
      in_str = "";
      cmd_available = false;
    }
    else
    {
      check_display_restore();
      process_key();
      reset_keyboard();
      in_str = "";
    }
  }

  do_server();

  check_rotary_encoder();

  timer_loop();

  if (cnt++ > 1)
  {
    toggle = !toggle;
    int val = toggle ? 0 : 255;
    analogWrite(LEDG, val);
    cnt = 0;
  }
}
