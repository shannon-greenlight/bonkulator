/*
  RotaryEncoder.h - RotaryEncoder library for Wiring - implementation
  Copyright (c) 2019 Shannon Vance.  All right reserved.
*/

// include core Wiring API
#include <Arduino.h>

// include this library's description file
#include "RotaryEncoder.h"

// include description files for other libraries used (if any)

// Constructor /////////////////////////////////////////////////////////////////
// Function that handles the creation and setup of instances

RotaryEncoder::RotaryEncoder()
{
  // initialize this instance's variables
  state = REST;
  setEncoderValue(0);

  pinMode(msb_pin, INPUT);     // MSB - A terminal
  pinMode(lsb_pin, INPUT);     // LSB - B terminal
  digitalWrite(msb_pin, HIGH); //turn pullup resistor on
  digitalWrite(lsb_pin, HIGH); //turn pullup resistor on
}

// Public Methods //////////////////////////////////////////////////////////////
// Functions available in Wiring sketches, this library, and other libraries
void RotaryEncoder::incEncoder()
{
  encoderValue++;
  if (numFxns > 0)
    encoderValue %= numFxns;
}

void RotaryEncoder::decEncoder()
{
  encoderValue--;
  if (numFxns > 0 && encoderValue < 0)
    encoderValue = numFxns - 1;
}

int RotaryEncoder::getEncoderValue()
{
  return encoderValue;
}

void RotaryEncoder::setEncoderValue(int val)
{
  encoderValue = val;
}

void RotaryEncoder::aChanInt()
{
  channel = "A";
  updateEncoder();
}

void RotaryEncoder::bChanInt()
{
  channel = "B";
  updateEncoder();
}

void RotaryEncoder::resetEncoder()
{
  setEncoderValue(0);
  last_encoder_val = encoderValue;
}

boolean RotaryEncoder::is_adjusting()
{
  return encoderValue != 0 && encoderValue != last_encoder_val;
}

void RotaryEncoder::updateEncoder()
{
  //t.print("");
  int MSB = digitalRead(msb_pin); //MSB = most significant bit - A terminal
  int LSB = digitalRead(lsb_pin); //LSB = least significant bit - B terminal
  //int state;
  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  //debugInt("Encoded: ",encoded);
  switch (state)
  {
  case REST:
    if (debug)
    {
      // t.setRow("10");
      // t.clrBelowCursor();
    }
    switch (encoded)
    {
    case 0:
      state = (channel == "A") ? CW : CCW; // we missed first event
      break;
    case 1:
      state = CCW;
      break;
    case 2:
      state = CW;
      break;
    case 3:
      state = REST; // we missed it all
      break;
    }
    break;
  case CW:
    switch (encoded)
    {
    case 0:
      state = UNK;
      break;
    case 1:
      state = UNK;
      break;
    case 2:
      state = UNK; //
      break;
    case 3:
      state = REST; // expected
      incEncoder();
      break;
    }
    break;
  case CCW:
    switch (encoded)
    {
    case 0:
      state = UNK;
      break;
    case 1:
      state = UNK;
      break;
    case 2:
      state = UNK; //
      break;
    case 3:
      state = REST; // expected
      decEncoder();
      break;
    }
    break;
  case UNK:
    //t.setRow("25");
    //t.println("Unknown state: "+String(encoded));
    if (encoded == 3)
      state = REST;
    break;
  }
  if (debug)
  {
    // t.setRow(String(15+encoded));
    // t.clrBelowCursor();
    // t.println("Encoded: "+String(encoded) + " State: "+ String(state)+ " Channel: "+channel);
  }
}

// Private Methods ////////////////////////////////////////////////////////////
// Functions only available to other functions in this library

RotaryEncoder e; // external