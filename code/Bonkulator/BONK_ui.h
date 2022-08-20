#include "GREENFACE_ui.h"
// display screen
#define TERMINAL_WIDTH 76 // Display area under title box
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
GREENFACE_ui ui(&display);

void terminal_print_status(bool force = false)
{
  static String old_status = "";
  int pos = TERMINAL_WIDTH / 2 - status_string.length() / 2;
  if (ui.terminal_mirror & (status_string != old_status || force))
  {
    ui.t.setCursor(STATUS_ROW, "1");
    ui.t.clrToEOL();
    ui.t.setCursor(STATUS_ROW, String(pos));
    ui.t.print(status_string);
    old_status = status_string;
  }
}

void terminalSplash()
{
  // Use Putty to communicate (VT100 terminal)
  ui.t.clrScreen();
  ui.t.setCursor("1", "1");
  const int stars = TERMINAL_WIDTH;

  ui.t.printChars(stars, "*");
  ui.t.println("");
  ui.t.printTitle(stars, F("The Bonkulator  by  GREENFACE LABS "));
  ui.t.setCursor("2", String(stars));
  ui.t.println("*");
  ui.t.printTitle(stars, String(VERSION_NUM) + " " + settings_get_device_name());
  ui.t.setCursor("3", String(stars));
  ui.t.println("*");
  ui.t.printChars(stars, "*");
  ui.t.println("");
}

void newFxn(String fxn)
{
  ui.clearDisplay();
  ui.t.clrDown(FXN_ROW);
  terminalSplash();
  ui.printLine(fxn, 0, 2);
}

void restore_display()
{
  ui.all_on();
  ui.reset_inactivity_timer();
  selected_fxn->display();
  terminal_print_status(true);
}

void splash()
{
  ui.clearDisplay();
  ui.printText(F("The Bonkulator"), 0, 0, 1);
  ui.printText(F("_______________"), 0, 8, 1);
  ui.printText(VERSION_NUM "", 0, 16, 1);
  ui.printText(F("Hi, my name is:"), 0, 32, 1);
  ui.printText(settings_get_device_name(), 0, 40, 1);

  terminalSplash();
}

void gen_params_macro(SPANK_fxn *item, bool print_header = true)
{
  if (print_header)
  {
    ui.t.clrScreen();
    ui.terminal_debug("// Macro for: " + item->name);
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
