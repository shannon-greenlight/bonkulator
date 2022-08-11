#include "Wire.h"

typedef struct
{
    uint16_t *waveform_data;
    byte data_len;
    byte current_index;
    unsigned long next_time;
    uint16_t output_value;
    int16_t randomness_factor; // range: -1023 -> 1023
    float scale;               // range: 0 - 100, for CV
    int16_t offset;            // range: -4095 -> 4095, for CV
    uint16_t time_inc;         // range: 1ms - 32767ms
    uint16_t repeat_count;
    bool ok_set_idle;
} OutputData;

uint16_t waveform0_ref[WAVEFORM_PARTS];
uint16_t waveform1_ref[WAVEFORM_PARTS];
uint16_t waveform2_ref[WAVEFORM_PARTS];
uint16_t waveform3_ref[WAVEFORM_PARTS];
uint16_t waveform4_ref[WAVEFORM_PARTS];
uint16_t waveform5_ref[WAVEFORM_PARTS];
uint16_t waveform6_ref[WAVEFORM_PARTS];
uint16_t waveform7_ref[WAVEFORM_PARTS];

uint16_t *waveform_reference[8] = {
    waveform0_ref,
    waveform1_ref,
    waveform2_ref,
    waveform3_ref,
    waveform4_ref,
    waveform5_ref,
    waveform6_ref,
    waveform7_ref,
};

uint16_t waveform0_data[WAVEFORM_PARTS];
uint16_t waveform1_data[WAVEFORM_PARTS];
uint16_t waveform2_data[WAVEFORM_PARTS];
uint16_t waveform3_data[WAVEFORM_PARTS];
uint16_t waveform4_data[WAVEFORM_PARTS];
uint16_t waveform5_data[WAVEFORM_PARTS];
uint16_t waveform6_data[WAVEFORM_PARTS];
uint16_t waveform7_data[WAVEFORM_PARTS];

OutputData outputs[8] =
    {
        // waveform_data, data_len, current_index, next_time, output_value, randomness_factor, scale, offset, time_inc, repeat_count, ok_set_idle
        {waveform0_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 10, 0, false},
        {waveform1_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 0, 0, false},
        {waveform2_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 0, 0, false},
        {waveform3_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 0, 0, false},
        {waveform4_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 0, 0, false},
        {waveform5_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 0, 0, false},
        {waveform6_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 0, 0, false},
        {waveform7_data, WAVEFORM_PARTS, 0, 0, 0, 0, 0, 0, 0, 0, false},
};
