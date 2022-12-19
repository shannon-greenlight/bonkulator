#include <Wire.h>

#define OUTPUT_CAL_OUTPUT_NUM 0
#define OUTPUT_CAL_OFFSET 1
#define OUTPUT_CAL_SCALE 2
#define OUTPUT_CAL_STATE 3
#define OUTPUT_CAL_NUM_PARAMS 4

uint16_t _output_cal_params[OUTPUT_CAL_NUM_PARAMS];
uint16_t _output_cal_mins[] = {0, 0, 0, 0};
uint16_t _output_cal_maxs[] = {NUM_OUTPUTS - 1, DAC_FS, 1000, 1};
uint16_t _output_cal_init_vals[] = {0, DAC_MID, 980, 0};
uint16_t *output_cal_stuff[] = {_output_cal_params, _output_cal_mins, _output_cal_maxs, _output_cal_init_vals};
String output_cal_labels[] = {"Output: ", "Offset: ", "Scale: ", "Calibrated: "};
String output_cal_string_params[] = {"", "", "", "No ,Yes"};
int16_t output_cal_offsets[] = {0, -DAC_MID, 0}; // allows negative numbers

Greenface_gadget output_cal_fxn("Calibrate", output_cal_labels, output_cal_stuff, sizeof(_output_cal_params) / sizeof(_output_cal_params[0]));

// output corrections
int16_t _output_scale_corrections[NUM_OUTPUTS] = {0, 0, 0, 0, 0, 0, 0, 0};
int16_t _output_offset_corrections[NUM_OUTPUTS] = {0, 0, 0, 0, 0, 0, 0, 0};

EEPROM_ArrInt output_scale_corrections(_output_scale_corrections, NUM_OUTPUTS);
EEPROM_ArrInt output_offset_corrections(_output_offset_corrections, NUM_OUTPUTS);
EEPROM_String output_calibrated(NUM_OUTPUTS);

// void (*func_ptr[3])() = {fun1, fun2, fun3};

int get_raw_output_offset_correction(int output)
{
    // Serial.println("Get raw offset correction: for: " + String(output));
    return _output_offset_corrections[output];
}

int get_output_offset_correction(int output)
{
    return output_offset_corrections.get(output) + DAC_MID;
}

int get_output_scale_correction(int output)
{
    // Serial.println("Get scale correction: for: " + String(output));
    return output_scale_corrections.get(output);
}

bool get_output_calibrated(int output)
{
    // Serial.println("Get output calibrated: output: " + String(output) + " val: " + output_calibrated.get());
    return output_calibrated.charAt(output) == 'Y';
}

void get_output_corrections()
{
    int output = output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM);
    output_cal_fxn.param_put(get_output_offset_correction(output), OUTPUT_CAL_OFFSET);
    output_cal_fxn.param_put(get_output_scale_correction(output), OUTPUT_CAL_SCALE);
    output_cal_fxn.param_put(get_output_calibrated(output) ? 1 : 0, OUTPUT_CAL_STATE);
}

void set_output_offset_correction(int val)
{
    int output = output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM);
    // Serial.println("Set offset correction: " + String(val) + " for: " + String(output));
    output_offset_corrections.put(val, output);
    update_waveform();
}

void set_output_scale_correction(int val)
{
    int output = output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM);
    // Serial.println("Set scale correction: " + String(val) + " for: " + String(output));
    output_scale_corrections.put(val, output);
    update_waveform();
}

void set_output_calibrated(int val)
{
    int output = output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM);
    // Serial.println("Set output calibrated: " + String(val) + " for: " + String(output));
    output_calibrated.putCharAt(val == 1 ? 'Y' : 'N', output);
}

void output_cal_update_params()
{
    // ui.terminal_debug("Update params: ");
    get_output_corrections(); // sets params for selected output from EEPROM
}

void output_cal_update_offset()
{
    int offset = output_cal_fxn.get_param_w_offset(OUTPUT_CAL_OFFSET);
    // ui.terminal_debug("Offset correction: " + String(offset));
    set_output_offset_correction(offset);
    dac_out(output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM), DAC_MID + offset);
}

void output_cal_update_scale()
{
    int scale = output_cal_fxn.get_param(OUTPUT_CAL_SCALE);
    int offset = output_cal_fxn.get_param_w_offset(OUTPUT_CAL_OFFSET);
    int dac_val = ((DAC_FS - DAC_MID) * scale / 1000.0) + DAC_MID;
    dac_val += offset;
    dac_out(output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM), dac_val);
    // ui.terminal_debug("Scale correction: " + String(scale) + " DAC: " + String(dac_val));
    set_output_scale_correction(scale);
}

void output_cal_update_state()
{
    // (*bonk_outputs[output_cal_fxn.get_param(OUTPUT_CAL_OUTPUT_NUM)]).put_param(0, OUTPUT_IDLE_VALUE);
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

void output_corrections_init()
{
    output_scale_corrections.fill(IDEAL_SCALE_CORRECTION);
    output_scale_corrections.init();
    output_offset_corrections.fill(0);
    output_offset_corrections.init();
    output_calibrated.init();
    output_calibrated.put("NNNNNNNN");
    get_output_corrections();
}

void output_cal_init()
{
    output_cal_fxn.init();
}

void output_cal_begin()
{
    output_scale_corrections.begin(false);
    output_scale_corrections.xfer();
    output_offset_corrections.begin(false);
    output_offset_corrections.xfer();
    output_calibrated.begin(false);
    output_calibrated.xfer();

    output_cal_fxn.begin();
    output_cal_fxn.string_params = output_cal_string_params;
    output_cal_fxn.offsets = output_cal_offsets;
    output_cal_fxn.update_fxns = output_cal_update_fxns;
    output_cal_fxn.display_fxn = &output_cal_display;
}

void output_cal_params_macro()
{
    // print out code to get to cal fxn
    String prefix = "*\r\np7\r\n1\r\n!";

    Greenface_gadget *fxn = &output_cal_fxn;
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        output_cal_fxn.param_put(i, OUTPUT_CAL_OUTPUT_NUM);
        output_cal_update_params();
        output_cal_update_offset();
        output_cal_update_scale();
        output_cal_update_state();
        gen_params_macro(fxn, i == 0, prefix);
    }
    outputs_begin();
    Serial.println("*");
}
