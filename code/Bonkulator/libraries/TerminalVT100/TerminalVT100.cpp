/*
  TerminalVT100.h - TerminalVT100 library
  Copyright (c) 2019 Shannon Vance.  All right reserved.
  https://en.wikipedia.org/wiki/ANSI_escape_code
*/

// include core API
#include <Arduino.h>

// include this library's description file
#include "TerminalVT100.h"

// Constructor /////////////////////////////////////////////////////////////////
// Function that handles the creation and setup of instances

TerminalVT100::TerminalVT100()
{
}

// Public Methods //////////////////////////////////////////////////////////////
// Functions available in Wiring sketches, this library, and other libraries
void TerminalVT100::begin()
{
  // initialize this instance's variables

  Serial.begin(9600);
  // do {
  //   ;
  // } while(!Serial);

  // Use Putty to communicate (VT100 terminal)
  Serial.print("\eSP F");  // tell to use 7-bit control codes
  Serial.print("\e[?25l"); // hide cursor
  Serial.print("\e[?12l"); // disable cursor highlighting
  clrScreen();
}

void TerminalVT100::print(String s)
{
  Serial.print(s);
}

void TerminalVT100::println(String s)
{
  Serial.println(s);
}

void TerminalVT100::clrToEOL()
{
  print("\e[0K");
}

void TerminalVT100::setCursor(String row, String col)
{
  print("\e[" + row + ";" + col + "H");
  //print("\e[2K");
}

void TerminalVT100::setRow(String row)
{
  print("\e[" + row + ";1H");
  //print("\e[2K");
}

void TerminalVT100::clrScreen()
{
  print("\e[2J"); // clear screen
}

void TerminalVT100::clrBelowCursor()
{
  print("\e[J"); // clear screen
}

void TerminalVT100::clrDown(String row)
{
  setRow(row);
  print("\e[J"); // clear screen
}

void TerminalVT100::blinkOn()
{
  print("\e[5m");
}

void TerminalVT100::blinkOff()
{
  print("\e[0m");
}

void TerminalVT100::printChars(int nchars, String s)
{
  for (int i = 0; i < nchars; i++)
  {
    print(s);
  }
}

void TerminalVT100::printTitle(int width, String title)
{
  float spaces = (((width - 2) - title.length()) / 2);
  //println("");
  print("*");
  printChars(floor(spaces), " ");
  print(title);
  // printChars(ceil(spaces), " ");
  // println("*");
  // println("Spaces: " + String(spaces));
}

void TerminalVT100::printVal(String row, String label, String val)
{
  setRow(row);
  print(label);
  clrToEOL();
  println(val);
}

// Private Methods /////////////////////////////////////////////////////////////
// Functions only available to other functions in this library
