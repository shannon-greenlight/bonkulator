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
