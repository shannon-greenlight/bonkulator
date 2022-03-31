// Rotary Encoder
#include <RotaryEncoder.h>

void intFxnA(void)
{
  e.aChanInt();
}

void intFxnB(void)
{
  e.bChanInt();
}

void set_encoder()
{
  switch (settings_get_encoder_type())
  {
  case 0:                     // bourns
    e.msb_pin = ROTARY_B_PIN; // A7
    e.lsb_pin = ROTARY_A_PIN; // A6
    break;
  case 1: // amazon
    e.msb_pin = ROTARY_A_PIN;
    e.lsb_pin = ROTARY_B_PIN;
    break;
  }

  // now set up interrupts
  attachInterrupt(digitalPinToInterrupt(e.lsb_pin), intFxnB, RISING);
  attachInterrupt(digitalPinToInterrupt(e.msb_pin), intFxnA, RISING);
}
