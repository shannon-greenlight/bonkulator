#include "SPANK_fxn.h"

#define OUTPUT_TIME_INC 0
#define OUTPUT_DELAY 1
#define OUTPUT_REPEAT 2
#define OUTPUT_ENABLE_T0 3
#define OUTPUT_ENABLE_T1 4
#define OUTPUT_ENABLE_T2 5
#define OUTPUT_ENABLE_T3 6
#define OUTPUT_WAVEFORM 7

#define NUM_OUTPUTS 8

uint16_t _output_mins[] = {1, 0, 0, 0, 0, 0, 0, 0};
uint16_t _output_maxs[] = {32000, 32000, 1, 1, 1, 1, 1, 3};
uint16_t _output_init_vals[] = {10, 10, 0, 0, 0, 0, 0, 0};
String output_labels[] = {"Time Inc: ", "Delay: ", "Repeat: ", "Enable T0: ", "Enable T1: ", "Enable T2: ", "Enable T3: ", "Waveform: "};
String output_string_params[] = {"", "", "", "", "", "", "", "Sine,Inv Sine,Ramp Up,Ramp Down"};

// Output definitions
uint16_t _output0_params[8];
uint16_t *output0_stuff[] = {_output0_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output0("Output 0", output_labels, output0_stuff, sizeof(_output0_params) / sizeof(_output0_params[0]), &ui);

uint16_t _output1_params[8];
uint16_t *output1_stuff[] = {_output1_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output1("Output 1", output_labels, output1_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]), &ui);

uint16_t _output2_params[8];
uint16_t *output2_stuff[] = {_output2_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output2("Output 2", output_labels, output2_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]), &ui);

uint16_t _output3_params[8];
uint16_t *output3_stuff[] = {_output3_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output3("Output 3", output_labels, output3_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]), &ui);

uint16_t _output4_params[8];
uint16_t *output4_stuff[] = {_output4_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output4("Output 4", output_labels, output4_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]), &ui);

uint16_t _output5_params[8];
uint16_t *output5_stuff[] = {_output5_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output5("Output 5", output_labels, output5_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]), &ui);

uint16_t _output6_params[8];
uint16_t *output6_stuff[] = {_output6_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output6("Output 6", output_labels, output6_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]), &ui);

uint16_t _output7_params[8];
uint16_t *output7_stuff[] = {_output7_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output7("Output 7", output_labels, output7_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]), &ui);

SPANK_fxn *bonk_outputs[8] = {
    &output0,
    &output1,
    &output2,
    &output3,
    &output4,
    &output5,
    &output6,
    &output7,
};

void display_output()
{
    (*bonk_outputs[selected_output.get()]).display();
}

void init_all_outputs()
{
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        (*bonk_outputs[i]).init();
    }
}

void begin_all_outputs()
{
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        (*bonk_outputs[i]).begin();
    }
}
