// Dude, ensure this library description is only included once
#ifndef Hardware_defs_h
#define Hardware_defs_h

#define ROTARY_P_PIN 4
#define ROTARY_A_PIN 5
#define ROTARY_B_PIN 6

#define CS_PIN 7
#define DOUT_PIN 8
#define CLK_PIN 9

// Triggers
#define T0_PIN 10
#define T1_PIN 11
#define T2_PIN 12
#define T3_PIN 13

// Trigger LEDs
#define T0_LED 0
#define T1_LED 1
#define T2_LED 2
#define T3_LED 3

#define DAC_FS 4095
#define DAC_MID 2048
#define ADC_FS 4095
#define ADC_FS_OVER_2 2048
#define ADC_FS_OVER_4 1024
#define IDEAL_SCALE_CORRECTION 976 // for correcting DAC outputs. .9736 is 5.478 / 5.333 which is desired FS / board FS

#define NUM_INPUTS 2
#define NUM_OUTPUTS 8
#define WAVEFORM_PARTS 128

#define ACTIVATE_STRING "Push Activate"

#endif
