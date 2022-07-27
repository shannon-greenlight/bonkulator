#include <Wire.h>

#define OUTPUT_CAL_OUTPUT_NUM 0
#define OUTPUT_CAL_OFFSET 1
#define OUTPUT_CAL_SCALE 2
#define OUTPUT_CAL_STATE 3
#define OUTPUT_CAL_NUM_PARAMS 4

#define OUTPUT_CAL_OFFSET_CORRECTION -512

uint16_t _output_cal_params[OUTPUT_CAL_NUM_PARAMS];
uint16_t _output_cal_mins[] = {0, 0, 0, 0};
uint16_t _output_cal_maxs[] = {NUM_OUTPUTS - 1, DAC_FS, 10000, 1};
uint16_t _output_cal_init_vals[] = {0, 512, 9763, 0};
uint16_t *output_cal_stuff[] = {_output_cal_params, _output_cal_mins, _output_cal_maxs, _output_cal_init_vals};
String output_cal_labels[] = {"Output: ", "Offset: ", "Scale: ", "Calibrated: "};
String output_cal_string_params[] = {"", "", "", "No ,Yes"};
int16_t output_cal_offsets[] = {0, OUTPUT_CAL_OFFSET_CORRECTION, 0}; // allows negative numbers

SPANK_fxn output_cal_fxn("Calibrate", output_cal_labels, output_cal_stuff, sizeof(_output_cal_params) / sizeof(_output_cal_params[0]), &ui);

// void (*func_ptr[3])() = {fun1, fun2, fun3};

void output_cal_update_params()
{
    // ui.terminal_debug("Update params: ");
    get_corrections(); // sets params for selected output from EEPROM
}

void output_cal_update_offset()
{
    int offset = output_cal_fxn.get_param_w_offset(OUTPUT_CAL_OFFSET);
    // ui.terminal_debug("Offset correction: " + String(offset));
    set_offset_correction(offset);
    // dac_out(output_cal_fxn.get_param_w_offset(OUTPUT_CAL_OUTPUT_NUM), offset);
    dac_out(output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM), 512 + offset);
}

void output_cal_update_scale()
{
    // (*bonk_outputs[output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM)]).put_param(0, OUTPUT_DACVAL);
    int scale = output_cal_fxn.get_param(OUTPUT_CAL_SCALE);
    int offset = output_cal_fxn.get_param_w_offset(OUTPUT_CAL_OFFSET);
    // int dac_val = ((float)scale / 10000.0) * DAC_FS;
    int dac_val = ((DAC_FS - DAC_MID) * scale / 10000.0) + DAC_MID;
    dac_val += offset;
    dac_out(output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM), dac_val);
    // ui.terminal_debug("Scale correction: " + String(scale) + " DAC: " + String(dac_val));
    set_scale_correction(scale);
}

void output_cal_update_state()
{
    // (*bonk_outputs[output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM)]).put_param(0, OUTPUT_DACVAL);
    int state = output_cal_fxn.get_param(OUTPUT_CAL_STATE);
    // ui.terminal_debug("Output calibrated: " + String(state));
    set_output_calibrated(state);
}

update_fxn output_cal_update_fxns[OUTPUT_CAL_NUM_PARAMS] = {output_cal_update_params, output_cal_update_offset, output_cal_update_scale, output_cal_update_state};

void output_cal_display()
{
    // ui.terminal_debug("output_cal_display param_num: " + String(output_cal_fxn.param_num));
    switch (output_cal_fxn.param_num)
    {
    case OUTPUT_CAL_OFFSET:
        output_cal_update_offset();
        break;
    case OUTPUT_CAL_SCALE:
        output_cal_update_scale();
        break;
    }
    output_cal_fxn.default_display();
}

void output_cal_init()
{
    output_cal_fxn.init();
}

void output_cal_begin()
{
    output_cal_fxn.begin();
    output_cal_fxn.string_params = output_cal_string_params;
    output_cal_fxn.offsets = output_cal_offsets;
    output_cal_fxn.update_fxns = output_cal_update_fxns;
    output_cal_fxn.display_fxn = &output_cal_display;
}