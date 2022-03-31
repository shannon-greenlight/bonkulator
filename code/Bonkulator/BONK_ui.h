#include "GREENFACE_ui.h"
// display screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
GREENFACE_ui ui(&display);

void terminal_print_status(bool repeat_on, bool triggered, float scale, int offset)
{
  if (ui.terminal_mirror)
  {
    ui.t.setCursor(STATUS_ROW, "33");
    ui.t.print(triggered ? "TRIGGERED" : "         ");
    String rpeat = repeat_on ? "ON " : "OFF";
    ui.t.setCursor(FXN_ROW, "16");
    ui.t.print("Repeat: " + rpeat);
    ui.t.setCursor(FXN_ROW, "28");
    float percent = 100 * scale;
    ui.t.print("CV Scale: " + String(percent) + String("%"));
    ui.t.print(" Offset: " + String(offset));
  }
}

void terminalSplash()
{
  // Use Putty to communicate (VT100 terminal)
  ui.t.clrScreen();
  ui.t.setCursor("1", "1");
  const int stars = 76;

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
