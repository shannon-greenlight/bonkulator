// display screen
#define PRODUCT_NAME "The Bonkulator"
#include <Greenface_ui.h>
#define TERMINAL_WIDTH 76 // Display area under title box
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Greenface_ui ui(&display, PRODUCT_NAME);
enum
{
  OUTPUT_WAVEFORM,
  OUTPUT_INIT_DELAY,
  OUTPUT_ACTIVE_TIME,
  OUTPUT_IDLE_TIME,
  OUTPUT_REPEAT,
  OUTPUT_ENABLE_T0,
  OUTPUT_ENABLE_T1,
  OUTPUT_ENABLE_T2,
  OUTPUT_ENABLE_T3,
  OUTPUT_CV0,
  OUTPUT_CV1,
  OUTPUT_SCALE,
  OUTPUT_OFFSET,
  OUTPUT_RANDOMNESS,
  OUTPUT_QUANTIZE,
  OUTPUT_IDLE_VALUE,
  OUTPUT_CLOCK,
  OUTPUT_RANGE,
  OUTPUT_TRIG_CTRL,
  OUTPUT_TRIG_CTRL_VAL,
  NUM_OUTPUT_PARAMS
};

String old_status = "";
void terminal_print_status(String status_string, bool force = false)
{
  // static String old_status = "";
  // ui.terminal_debug("Status string: <" + status_string + ">");
  if (status_string.length() > 0)
  {
    int pos = TERMINAL_WIDTH / 2 - status_string.length() / 2;
    pos = pos + pos % 2;
    if (ui.terminal_mirror && (status_string != old_status || force))
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
  ui.all_on(!usb_direct_enabled());
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
bool sending_status = false;
void send_status_to_USB(String event = "trigger")
{
  if (!sending_to_USB && usb_direct_enabled())
  {
    sending_status = true;
    sending_to_USB = true;
    ui.t.print("{");
    ui.t.print(toJSON("event", event));
    ui.t.print(",");
    if (event == "trigger")
    {
      ui.t.print(toJSON("t0", trig0.state == TRIGGER_ACTIVE ? "ON" : "OFF"));
      ui.t.print(",");
      ui.t.print(toJSON("t1", trig1.state == TRIGGER_ACTIVE ? "ON" : "OFF"));
      ui.t.print(",");
      ui.t.print(toJSON("t2", trig2.state == TRIGGER_ACTIVE ? "ON" : "OFF"));
      ui.t.print(",");
      ui.t.print(toJSON("t3", trig3.state == TRIGGER_ACTIVE ? "ON" : "OFF"));
    }
    else if (event == "offset")
    {
      ui.t.print(toJSON("value", String(selected_fxn->get_param_w_offset(OUTPUT_OFFSET))));
    }
    else if (event == "scale")
    {
      ui.t.print(toJSON("value", String(selected_fxn->get_param_w_offset(OUTPUT_SCALE))));
    }
    else if (event == "Active Time")
    {
      ui.t.print(toJSON("value", String(selected_fxn->get_param_w_offset(OUTPUT_ACTIVE_TIME))));
    }
    else if (event == "SampleTime")
    {
      ui.t.print(toJSON("value", String(selected_fxn->get_param_w_offset(OUTPUT_ACTIVE_TIME))));
    }
    else if (event == "randomness")
    {
      ui.t.print(toJSON("value", String(selected_fxn->get_param_w_offset(OUTPUT_RANDOMNESS))));
    }
    else if (event == "Idle Value")
    {
      ui.t.print(toJSON("value", String(selected_fxn->get_param_w_offset(OUTPUT_IDLE_VALUE))));
    }
    ui.t.print("}");
    // This terminates serialport message
    ui.t.println("\r\n\r\n");
    sending_to_USB = false;
  }
}

String globals_toJSON(char cmd)
{
  String out = "";
  out += (toJSON("fxn", selected_fxn->name));
  out += (",");
  out += (toJSON("fxn_num", String(remembered_fxn.get())));
  out += (",");
  if (true || cmd == ' ' || cmd == 'f' || cmd == '+' || cmd == '-' || cmd == 'U')
    out += (list_fxns());
  out += (toJSON("device_name", settings_get_device_name()));
  out += (",");
  out += (toJSON("cmd", String(cmd)));
  out += (",");

  uint8_t digit_num = selected_fxn->digit_num;
  if (selected_fxn->get_min_w_offset() < 0 && digit_num > 0)
  {
    digit_num--;
  }
  out += (toJSON("digit_num", String(digit_num)));
  out += (",");

  out += (toJSON("group", String(group)));
  out += (",");
  out += (toJSON("group_active", String(group_active)));
  out += (",");

  out += (toJSON("param_num", String(selected_fxn->param_num)));
  out += (",");
  out += (toJSON("param_active", String(selected_fxn->get_param_active())));
  out += (",");
  out += (toJSON("dp", (selected_fxn->decimal_places) ? String(selected_fxn->decimal_places[selected_fxn->param_num]) : "0"));
  out += (",");

  out += (toJSON("fs_volts", output_get_fs()));
  out += (",");
  out += (toJSON("fs_offset", output_get_fs_offset()));
  out += (",");
  out += (toJSON("offset_max", "100"));
  out += (",");
  out += (toJSON("offset_min", "-100"));
  out += (",");
  out += (toJSON("software_version", settings_get_version()));
  out += (",");
  out += (toJSON("dac_fs", String(DAC_FS)));
  out += (",");
  out += (toJSON("adc_fs", String(ADC_FS)));
  out += (",");
  return out;
}

String messages_toJSON()
{
  String out = "";
  if (selected_fxn_num != SETTINGS_FXN || in_user_waveforms())
  {
    set_wifi_message();
    out += (toJSON("message", waveform_values));
    out += (",");
  }
  waveform_values = "";

  out += (toJSON("system_message", system_message));
  out += (",");
  // system_message = "";

  if (in_wifi())
  {
    out += (wifi_toJSON());
  }
  return out;
}

String triggers_toJSON()
{
  String out = "";
  out += ("\"triggers\" : [");
  for (int trig_num = 0; trig_num < NUM_TRIGGERS; trig_num++)
  {
    if (trig_num > 0)
    {
      out += (",");
    }
    // ui.terminal_debug((*triggers[trig_num]).params_toJSON());
    out += ((*triggers[trig_num]).params_toJSON());
  }
  out += ("],");
  return out;
}

String fxn_params_toJSON()
{
  String out = "";
  out += ("\"params\" : [");
  if (!in_wifi() || sending_to_USB)
    out += (selected_fxn->params_toJSON());
  out += ("]");
  return out;
}

void send_data_to_USB(char cmd)
{
  if (sending_to_USB || !usb_direct_enabled())
    return;
  sending_to_USB = true;
  ui.t.print("{");
  // ui.terminal_debug("Send data to ui.t: " + String(selected_fxn->get_param(OUTPUT_WAVEFORM)));
  if (cmd == '[')
    return;
  ui.t.print(globals_toJSON(cmd));
  ui.t.print(messages_toJSON());
  ui.t.print(triggers_toJSON());
  ui.t.print(fxn_params_toJSON());

  ui.t.print("}");

  // This terminates serialport message
  ui.t.println("\r\n\r\n");
  sending_to_USB = false;
}
