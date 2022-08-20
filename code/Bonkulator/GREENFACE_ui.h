// ensure this library description is only included once
#ifndef GREENFACE_ui_h
#define GREENFACE_ui_h

#include <Adafruit_SSD1306.h> //
#include <Adafruit_GFX.h>     //
#include <Wire.h>             // ???
#include "TerminalVT100.h"

#include "version_num.h"
#include "hardware_defs.h"

// OLED Display
/*
HardWare I2C pins
A4   SDA
A5   SCL
*/
#define LINE_0 0
#define LINE_1 20
#define LINE_2 36
#define LINE_3 52

#define STATUS_ROW "5"
#define FXN_ROW "7"
#define DEBUG_ROW "2"

#define OLED_RESET 16 // Pin 15 -RESET digital signal
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16
extern String settings_get_device_name();

class GREENFACE_ui
{
public:
  bool display_is_on = true;
  bool terminal_mirror = true;
  String old_fxn = "";

  // todo use extern
  const int lines[4] = {LINE_1, LINE_2, LINE_3, LINE_0};
  TerminalVT100 t;
  Adafruit_SSD1306 *display;
  GREENFACE_ui(Adafruit_SSD1306 *_display)
  {
    display = _display;
  }

  void drawBitmap(int x, int y, const uint8_t *bitmap)
  {
    clearDisplay();
    // printText("Greenface",0,0,2);
    (*display).drawBitmap(x, y, bitmap, 128, 64, WHITE);
    showDisplay();
  }

  void begin(const uint8_t *bitmap)
  {
    t.begin();
    // SSD1306 Init
    (*display).begin(SSD1306_SWITCHCAPVCC, 0x3C); // Switch OLED
    // Clear the buffer.
    (*display).clearDisplay();
    (*display).stopscroll();
    (*display).setTextColor(WHITE, BLACK);
    //(*display).setRotation(2);
    //(*display).startscrollright(0,15);
    old_fxn = "";
    if (true)
    {
      for (int i = 0; i < 48; i += 2)
      {
        drawBitmap(0, 64 - i, bitmap);
      }
      // clearDisplay();
      fill(BLACK, 16);
      // printText("Greenface Labs",0,0,1);
      printText("Greenface Labs", 0, 0, 1);
      (*display).drawBitmap(0, 16, bitmap, 128, 64, WHITE);
      showDisplay();
      delay(3000);
      // clearDisplay();
      //  for (int i = 16; i <= 64; i += 4)
      //  {
      //    clearDisplay();
      //    //printText("Greenface",0,0,2);
      //    (*display).drawBitmap(0, i, bitmap, 128, 64, WHITE);
      //    showDisplay();
      //    //delay(2);
      //  }
    }
    reset_inactivity_timer();
    // showDisplay();
    // delay(2000);
  }

  void clearTerminal()
  {
    t.clrScreen();
  }

  void clearDisplay()
  {
    // clr terminal here
    if (terminal_mirror)
    {
      clearTerminal();
    }
    (*display).clearDisplay();
  }

  void showDisplay()
  {
    (*display).display();
  }

  void dim(boolean dim)
  {
    (*display).dim(dim);
  }

  void display_off()
  {
    display_is_on = false;
    (*display).ssd1306_command(SSD1306_DISPLAYOFF);
  }

  void display_on()
  {
    display_is_on = true;
    (*display).ssd1306_command(SSD1306_DISPLAYON);
  }

  unsigned long inactivity_timer = 0;

  void reset_inactivity_timer()
  {
    inactivity_timer = millis();
  }

  void check_inactivity_timer(int timeout)
  {
    if (millis() > (inactivity_timer + timeout * 60000) && display_is_on) // Screen Saver
    {
      screen_saver();
    }
  }

  void screen_saver()
  {
    fill(BLACK, 0);
    showDisplay();
    printLine(F("Screen Saver"), 0, 1);
    delay(1000);
    display_off();
  }

  void all_off()
  {
    display_off();
    terminal_mirror = false;
  }

  void all_on()
  {
    display_on();
    terminal_mirror = true;
  }

  void terminal_printRow(String text, String row)
  {
    if (row != "-1")
    {
      t.setRow(row);
      t.clrToEOL();
      t.println(text);
    }
  }

  void terminal_debug(String text)
  {
    terminal_printRow(text, DEBUG_ROW);
  }

  void terminal_printText(String text, int line)
  {
    String row = String(9 + ((line - 20) / 8));
    if (row == FXN_ROW)
    {
      // text = "Function: " + text;
    }
    terminal_printRow(text, row);
  }

  void printText(String s, int x = 0, int y = 0, int fontsize = 1)
  {
    if (display_is_on)
    {
      (*display).setCursor(x, y);
      (*display).setTextSize(fontsize);
      (*display).println(s);
      (*display).display();
    }
    if (terminal_mirror)
    {
      terminal_printText(s, y);
    }
  }

  void printScreen(String head, String body, String foot = "")
  {
    printText(head, 0, 0, 2);
    printText(body, 0, 20, 3);
    printText(foot, 0, 48, 2);
  }

  int calc_line(int line_num)
  {
    switch (line_num)
    {
    case 0:
      return LINE_0;
      break;
    case 1:
      return LINE_1;
      break;
    case 2:
      return LINE_2;
      break;
    case 3:
      return LINE_3;
      break;
    }
  }

  void printLine(String text, int line, int fontsize = 2)
  {
    printText(text, 0, line, fontsize);
    // terminalPrintParam(row, label, String(buf));
  }

  int constrainX(int x)
  {
    return constrain(x, 0, 127);
  }

  int constrainY(int y)
  {
    return constrain(y, LINE_1, 63);
  }

  void drawPixel(int x, int y, int color = WHITE, bool constrain_vals = false)
  {
    if (display_is_on)
    {
      if (constrain_vals)
      {
        x = constrainX(x);
        y = constrainX(y);
      }
      (*display).drawPixel(x, y, color);
    }
  }

  void drawHLine(int y, int color = WHITE)
  {
    if (display_is_on)
    {
      (*display).drawFastHLine(0, y, 128, color);
    }
  }

  void drawVLine(int x, int color = WHITE)
  {
    if (display_is_on)
    {
      (*display).drawFastVLine(x, LINE_1, 64, color);
    }
  }

  void drawVHash(int x, int y, int color = WHITE)
  {
    if (display_is_on)
    {
      (*display).drawFastVLine(x, constrainY(y - 4), 11, color);
    }
  }

  void drawXHash(int x, int y, int color = WHITE)
  {
    if (display_is_on)
    {
      // upper left
      drawPixel(x - 2, y - 2, WHITE, true);
      drawPixel(x - 1, y - 1, WHITE, true);

      // upper right
      drawPixel(x + 2, y - 2, WHITE, true);
      drawPixel(x + 1, y - 1, WHITE, true);

      // lower left
      drawPixel(x - 2, y + 2, WHITE, true);
      drawPixel(x - 1, y + 1, WHITE, true);

      // lower right
      drawPixel(x + 2, y + 2, WHITE, true);
      drawPixel(x + 1, y + 1, WHITE, true);
    }
  }

  void hiliteLine(uint16_t line_num, int num_lines = 3, int offset = 9)
  {
    String s = "---------------------";
    String blank = "                     ";
    for (int i = 0; i < num_lines; i++)
    {
      drawHLine(lines[i] + offset, BLACK);
      if (terminal_mirror)
      {
        terminal_printText(blank, lines[i] + offset);
      }
    }
    // Serial.println("Hilighting: "+String(lines[line_num]+offset));
    drawHLine(lines[line_num] + offset, WHITE);
    if (terminal_mirror)
    {
      terminal_printText(s, lines[line_num] + offset);
    }
  }

  void hiliteChar(uint16_t line_num, uint8_t char_num)
  {
    int num_lines = 3;
    int offset = 9;
    for (int i = 0; i < num_lines; i++)
    {
      drawHLine(lines[i] + offset, BLACK);
    }
    // Serial.println("Hilighting: " + String(lines[line_num] + offset));
    printText("_", char_num, lines[line_num] + offset, 1);
    // drawHLine(lines[line_num]+offset,WHITE);
  }

  void fill(int color = WHITE, int offset = 0)
  {
    for (int i = offset; i < 64; i++)
    {
      (*display).drawFastHLine(0, i, 128, color);
    }
    // t.clrDown(String((offset / 16) + 6));
    //  terminal_debug("fill offset: " + String(offset));
  }

  void graphData(int *data)
  {
    for (int i = 0; i < 128; i++)
    {
      drawPixel(i, data[i], WHITE);
    }
  }

  int fit_data(int intval, int scale)
  {
    float val = float(intval) / scale;
    return max(16, 63 - (int(48 * val)));
  }

  void graph_waveform(uint16_t *waveform_data, byte data_len)
  {
    // ui.terminal_debug("Graph waveform: " + String(output_num));
    int data[WAVEFORM_PARTS];

    int index = 0; // index into source waveform data
    float index_inc = WAVEFORM_PARTS / float(data_len);

    fill(BLACK, 16);

    for (int i = 0; i < 128; i++)
    {
      data[i] = fit_data(waveform_data[index], DAC_FS);
      if (i + 1 > (index + 1) * index_inc)
      {
        index++;
      }
    }
    graphData(data);
    showDisplay();
  }

  const int basex = 116;
  const int basey = 14;

  void printWiFiBar(int x, int h)
  {
    if (display_is_on)
    {
      (*display).drawFastVLine(x, basey - h, h, WHITE);
      (*display).drawFastVLine(x + 1, basey - h, h, WHITE);
    }
  }

  void printWiFiBars(long rssi)
  {
    if (rssi > -80)
      printWiFiBar(basex, 3);
    if (rssi > -70)
      printWiFiBar(basex + 3, 6);
    if (rssi > -60)
      printWiFiBar(basex + 6, 9);
    if (rssi > -50)
      printWiFiBar(basex + 9, 12);
    showDisplay();
  }

  void test()
  {
    fill();
    showDisplay();
  }

  void terminalPrintParam(String row, String label, String val)
  {
    t.printVal(row, label, val);
  }

  void terminalSplash()
  {
    // Use Putty to communicate (VT100 terminal)
    t.clrScreen();
    t.setCursor("1", "1");
    const int stars = 76;

    t.printChars(stars, "*");
    t.println("");
    t.printTitle(stars, F("The Bonkulator  by  GREENFACE LABS "));
    t.setCursor("2", String(stars));
    t.println("*");
    t.printTitle(stars, String(VERSION_NUM) + " " + settings_get_device_name());
    t.setCursor("3", String(stars));
    t.println("*");
    t.printChars(stars, "*");
    t.println("");
  }

  void newFxn(String fxn)
  {
    if (fxn != old_fxn)
    {
      clearDisplay();
      t.clrDown(FXN_ROW);
      terminalSplash();
      printLine(fxn, 0, fxn.length() > 9 ? 1 : 2);
    }
  }

  String format_float(float val, int digits, const char *format = "%.2f")
  {
    char buf[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // buffer to print up to 9 digits
    // String format = "%"+String(digits)+"d";
    // Serial.println("Format: "+String(format));
    snprintf(buf, digits + 1, format, val);
    return String(buf);
  }

  void printParam(String label, int param, int digits, const char *format, uint16_t line_num, int font_size = 1, String suffix = "")
  {
    char buf[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // buffer to print up to 9 digits
    // String format = "%"+String(digits)+"d";
    // Serial.println("Format: "+String(format));
    snprintf(buf, digits + 1, format, param);
    printLine(label + String(buf) + suffix, lines[line_num], font_size);
  }

  void underline_char(byte char_num, byte line_num, int font_size = 2, int offset = 0, const char *c = "-")
  {
    String prefix = "";
    for (int i = 0; i < char_num; i++)
    {
      prefix += " ";
    }
    String sval = prefix + c;
    // Serial.println("Prefix: " + prefix);
    // drawHLine(lines[line_num] + offset, BLACK);
    // fill(BLACK, lines[line_num] + offset);
    int num_blanks = 20 - sval.length();
    for (int i = 0; i < num_blanks; i++)
    {
      sval += " ";
    }
    printLine(sval, lines[line_num] + offset, font_size);
    //(*display).println(prefix + "-");
  }
};
#endif
