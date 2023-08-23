// display screen
#define PRODUCT_NAME "The Bonkulator"
#include <Greenface_ui.h>
#define TERMINAL_WIDTH 76 // Display area under title box
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Greenface_ui ui(&display, PRODUCT_NAME);

String old_status = "";
void terminal_print_status(String status_string, bool force = false)
{
  // static String old_status = "";
  // ui.terminal_debug("Status string: <" + status_string + ">");
  if (status_string.length() > 0)
  {
    int pos = TERMINAL_WIDTH / 2 - status_string.length() / 2;
    pos = pos + pos % 2;
    if (ui.terminal_mirror & (status_string != old_status || force))
    {
      // ui.t.setCursor(STATUS_ROW, "1");
      // ui.t.clrToEOL();
      ui.t.setCursor(STATUS_ROW, String(pos));
      ui.t.print(status_string);
      old_status = status_string;
    }
  }
}

uint16_t get_chars_per_line()
{
  int16_t x1, y1;
  uint16_t w, h;

  ui.display->getTextBounds(" ", 0, 0, &x1, &y1, &w, &h);
  return SCREEN_WIDTH / w;
  // Serial.println("Char width: " + String(w));
  // Serial.println("Chars / line: " + String(SCREEN_WIDTH / w));
}

void restore_display()
{
  ui.all_on();
  ui.reset_inactivity_timer();
  selected_fxn->display();
  terminal_print_status(old_status, true);
}

void gen_params_macro(Greenface_gadget *item, bool print_header = true, String prefix = "")
{
  if (print_header)
  {
    ui.t.clrScreen();
    ui.terminal_debug("// Macro for: " + item->name);
    Serial.println(prefix);
  }
  uint8_t param_type;
  for (int i = 0; i < item->num_params; i++)
  {
    param_type = item->get_param_type(i);
    // Serial.println(item->labels[i] + String(item->get_param_w_offset(i)) + " type: " + String(param_type));
    switch (param_type)
    {
    case SPANK_INT_PARAM_TYPE:
    case SPANK_STRING_PARAM_TYPE:
      Serial.println("p" + String(i));
      Serial.println(String(item->get_param_w_offset(i)));
      break;
    case SPANK_STRING_VAR_TYPE:
      Serial.println("p" + String(i));
      Serial.println("$" + String(item->get_param_as_string_var(i)));
      break;
    }
  }
}

bool sending_to_USB;
void send_status_to_USB()
{
  if (!sending_to_USB)
  {
    sending_to_USB = true;
    ui.t.print("{");
    ui.t.print(toJSON("status", "trigger"));
    ui.t.print(",");
    ui.t.print(toJSON("t0", trig0.state == TRIGGER_ACTIVE ? "ON" : "OFF"));
    ui.t.print(",");
    ui.t.print(toJSON("t1", trig1.state == TRIGGER_ACTIVE ? "ON" : "OFF"));
    ui.t.print(",");
    ui.t.print(toJSON("t2", trig2.state == TRIGGER_ACTIVE ? "ON" : "OFF"));
    ui.t.print(",");
    ui.t.print(toJSON("t3", trig3.state == TRIGGER_ACTIVE ? "ON" : "OFF"));
    ui.t.print("}");
    // This terminates serialport message
    ui.t.println("\r\n\r\n");
    sending_to_USB = false;
  }
}

void send_data_to_USB(char cmd)
{
  if (sending_to_USB)
    return;
  sending_to_USB = true;
  ui.t.print("{");
  // ui.terminal_debug("Send data to ui.t: " + String(selected_fxn->get_param(OUTPUT_WAVEFORM)));
  if (cmd == '[')
    return;
  ui.t.print(toJSON("fxn", selected_fxn->name));
  ui.t.print(",");
  ui.t.print(toJSON("fxn_num", String(remembered_fxn.get())));
  ui.t.print(",");
  if (cmd == ' ' || cmd == 'f' || cmd == '+' || cmd == '-' || cmd == 'U')
    ui.t.print(list_fxns());
  ui.t.print(toJSON("device_name", settings_get_device_name()));
  ui.t.print(",");
  ui.t.print(toJSON("cmd", String(cmd)));
  ui.t.print(",");

  uint8_t digit_num = selected_fxn->digit_num;
  if (selected_fxn->get_min_w_offset() < 0 && digit_num > 0)
  {
    digit_num--;
  }
  ui.t.print(toJSON("digit_num", String(digit_num)));
  ui.t.print(",");

  ui.t.print(toJSON("param_num", String(selected_fxn->param_num)));
  ui.t.print(",");
  ui.t.print(toJSON("param_active", String(selected_fxn->get_param_active())));
  ui.t.print(",");
  ui.t.print(toJSON("dp", (selected_fxn->decimal_places) ? String(selected_fxn->decimal_places[selected_fxn->param_num]) : "0"));
  ui.t.print(",");

  ui.t.print(toJSON("fs_volts", output_get_fs()));
  ui.t.print(",");
  ui.t.print(toJSON("fs_offset", output_get_fs_offset()));
  ui.t.print(",");
  ui.t.print(toJSON("offset_max", "100"));
  ui.t.print(",");
  ui.t.print(toJSON("offset_min", "-100"));
  ui.t.print(",");
  ui.t.print(toJSON("scale_min", "-100"));
  ui.t.print(",");
  ui.t.print(toJSON("scale_max", "100"));
  ui.t.print(",");
  ui.t.print(toJSON("dac_fs", String(DAC_FS)));
  ui.t.print(",");
  ui.t.print(toJSON("adc_fs", String(ADC_FS)));
  ui.t.print(",");

  if (selected_fxn_num != SETTINGS_FXN || in_user_waveforms())
  {
    set_wifi_message();
    ui.t.print(toJSON("message", wifi_ui_message));
    ui.t.print(",");
  }
  wifi_ui_message = "";

  if (in_wifi())
  {
    wifi_toJSON();
    // ui.t.print(toJSON("message", "A cool message..."));
    // ui.t.print(",");
  }

  ui.t.print("\"triggers\" : [");
  for (int trig_num = 0; trig_num < NUM_TRIGGERS; trig_num++)
  {
    if (trig_num > 0)
    {
      ui.t.print(",");
    }
    // ui.terminal_debug((*triggers[trig_num]).params_toJSON());
    ui.t.print((*triggers[trig_num]).params_toJSON());
  }
  ui.t.print("],");

  ui.t.print("\"params\" : [");
  if (!in_wifi())
    ui.t.print(selected_fxn->params_toJSON());
  ui.t.print("]");
  ui.t.print("}");

  // The HTTP response ends with another blank line:
  // ui.t.println("");

  // This terminates serialport message
  ui.t.println("\r\n\r\n");
  sending_to_USB = false;
}
