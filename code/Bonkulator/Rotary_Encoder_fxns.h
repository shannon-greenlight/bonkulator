// Rotary Encoder
// see https://playground.arduino.cc/Main/RotaryEncoders

#define encoder0PinA ROTARY_A_PIN
#define encoder0PinB ROTARY_B_PIN

volatile int encoder0Pos = 0;
volatile long encodeTime;
int oldPosition = 0;
uint8_t wait_time = 102;

void doEncoderA()
{
  if (millis() > encodeTime + wait_time)
  {
    // look for a low-to-high on channel A
    if (digitalRead(encoder0PinA) == HIGH)
    {

      // check channel B to see which way encoder is turning
      if (digitalRead(encoder0PinB) == LOW)
      {
        encoder0Pos += 1; // CW
      }
      else
      {
        encoder0Pos += -1; // CCW
      }
    }

    else // must be a high-to-low edge on channel A
    {
      // check channel B to see which way encoder is turning
      if (digitalRead(encoder0PinB) == HIGH)
      {
        encoder0Pos += 1; // CW
      }
      else
      {
        encoder0Pos += -1; // CCW
      }
    }
    encodeTime = millis();
    // Serial.println(encoder0Pos, DEC);
    // use for debugging - remember to comment out
  }
}

void doEncoderB()
{
  if (millis() > encodeTime + wait_time)
  {
    // look for a low-to-high on channel B
    if (digitalRead(encoder0PinB) == HIGH)
    {

      // check channel A to see which way encoder is turning
      if (digitalRead(encoder0PinA) == HIGH)
      {
        encoder0Pos += 1; // CW
      }
      else
      {
        encoder0Pos += -1; // CCW
      }
    }

    // Look for a high-to-low on channel B
    else
    {
      // check channel B to see which way encoder is turning
      if (digitalRead(encoder0PinA) == LOW)
      {
        encoder0Pos += 1; // CW
      }
      else
      {
        encoder0Pos += -1; // CCW
      }
    }
    encodeTime = millis();
  }
}

void set_encoder()
{
  if (settings_get_encoder_type())
  {
    attachInterrupt(ROTARY_B_PIN, doEncoderA, CHANGE);
    attachInterrupt(ROTARY_A_PIN, doEncoderB, CHANGE);
  }
  else
  {
    attachInterrupt(ROTARY_A_PIN, doEncoderA, CHANGE);
    attachInterrupt(ROTARY_B_PIN, doEncoderB, CHANGE);
  }
}

bool encoder_is_adjusting()
{
  bool adjusting = encoder0Pos != oldPosition;
  if (adjusting)
  {
    oldPosition = encoder0Pos;
  }
  return adjusting;
}

void resetEncoder()
{
  oldPosition = encoder0Pos = 0;
}

int getEncoderValue()
{
  return (encoder0Pos > 0) ? 1 : -1;
}

void rotary_encoder_begin()
{
  resetEncoder();
  set_encoder(); // sets msb,lsb for two types of encoder
}
