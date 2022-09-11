#include <Wire.h>

#define INPUT_CAL_INPUT_NUM 0
#define INPUT_CAL_OFFSET 1
#define INPUT_CAL_SCALE 2
#define INPUT_CAL_STATE 3
#define INPUT_CAL_NUM_PARAMS 4

#define INPUT_CAL_OFFSET_CORRECTION -2048

uint16_t _input_cal_params[INPUT_CAL_NUM_PARAMS];
uint16_t _input_cal_mins[] = {0, 0, 0, 0};
uint16_t _input_cal_maxs[] = {1, ADC_FS, 1300, 1};
uint16_t _input_cal_init_vals[] = {0, 0, 1000, 0};
uint16_t *input_cal_stuff[] = {_input_cal_params, _input_cal_mins, _input_cal_maxs, _input_cal_init_vals};
String input_cal_labels[] = {"Input: ", "Offset: ", "Scale: ", "Calibrated: "};
String input_cal_string_params[] = {"", "", "", "No ,Yes"};
int16_t input_cal_offsets[] = {0, INPUT_CAL_OFFSET_CORRECTION, 0}; // allows negative numbers

Greenface_gadget input_cal_fxn("Calibrate", input_cal_labels, input_cal_stuff, sizeof(_input_cal_params) / sizeof(_input_cal_params[0]));

// void (*func_ptr[3])() = {fun1, fun2, fun3};

// input corrections
int16_t _input_scale_corrections[] = {0, 0};
int16_t _input_offset_corrections[] = {0, 0};

EEPROM_ArrInt input_scale_corrections(_input_scale_corrections, NUM_INPUTS);
EEPROM_ArrInt input_offset_corrections(_input_offset_corrections, NUM_INPUTS);
EEPROM_String input_calibrated(NUM_INPUTS);

// raw saves time. No need to subtract INPUT_CAL_OFFSET_CORRECTION
int get_raw_input_offset_correction(int input)
{
    // Serial.println("Get raw input correction: for: " + String(input));
    return _input_offset_corrections[input];
}

int get_input_offset_correction(int input)
{
    // Serial.println("Get offset correction: for: " + String(input));
    return input_offset_corrections.get(input) - INPUT_CAL_OFFSET_CORRECTION;
}

int get_input_scale_correction(int input)
{
    // Serial.println("Get scale correction: for: " + String(input));
    return input_scale_corrections.get(input);
}

bool get_input_calibrated(int input)
{
    // Serial.println("Get input calibrated: for: " + String(input) + " val: " + input_calibrated.get());
    return input_calibrated.charAt(input) == 'Y';
}

void set_input_calibrated(int val)
{
    int input = input_cal_fxn.get_param(INPUT_CAL_INPUT_NUM);
    // Serial.println("Set output calibrated: " + String(val) + " for: " + String(output));
    input_calibrated.putCharAt(val == 1 ? 'Y' : 'N', input);
}

void get_input_corrections()
{
    int input = input_cal_fxn.get_param(INPUT_CAL_INPUT_NUM);
    // ui.terminal_debug("get_input_corrections: " + String(input));

    input_cal_fxn.param_put(get_input_offset_correction(input), INPUT_CAL_OFFSET);
    input_cal_fxn.param_put(get_input_scale_correction(input), INPUT_CAL_SCALE);
    input_cal_fxn.param_put(get_input_calibrated(input) ? 1 : 0, INPUT_CAL_STATE);
}

void input_cal_update_params()
{
    ui.terminal_debug("Update params: ");
    get_input_corrections(); // sets params for selected input from EEPROM
    input_cal_fxn.display();
}

void input_cal_update_offset()
{
    int offset = input_cal_fxn.get_param_w_offset(INPUT_CAL_OFFSET);
    int input = input_cal_fxn.get_param(INPUT_CAL_INPUT_NUM);
    input_offset_corrections.put(offset, input);
    // ui.terminal_debug("Offset correction: " + String(offset) + " for input " + String(input));
}

void input_cal_update_scale()
{
    int scale = input_cal_fxn.get_param(INPUT_CAL_SCALE);
    int input = input_cal_fxn.get_param(INPUT_CAL_INPUT_NUM);
    input_scale_corrections.put(scale, input);
}

void input_cal_update_state()
{
    // (*bonk_outputs[input_cal_fxn.get_param(INPUT_CAL_INPUT_NUM)]).put_param(0, INPUT_IDLE_VALUE);
    int state = input_cal_fxn.get_param(INPUT_CAL_STATE);
    ui.terminal_debug("Input calibrated: " + String(state));
    set_input_calibrated(state);
}

update_fxn input_cal_update_fxns[INPUT_CAL_NUM_PARAMS] = {input_cal_update_params, input_cal_update_offset, input_cal_update_scale, input_cal_update_state};

void input_cal_display()
{
    // ui.terminal_debug("input_cal_display param_num: " + String(input_cal_fxn.param_num));
    switch (input_cal_fxn.param_num)
    {
    case INPUT_CAL_OFFSET:
        input_cal_update_offset();
        break;
    case INPUT_CAL_SCALE:
        input_cal_update_scale();
        break;
    }
    input_cal_fxn.default_display();
}

void input_corrections_init()
{
    input_scale_corrections.fill(1000);
    input_scale_corrections.init();
    input_offset_corrections.fill(0);
    input_offset_corrections.init();
    input_calibrated.init();
    input_calibrated.put("NN");
    get_input_corrections();
}

void input_cal_init()
{
    input_cal_fxn.init();
}

void input_cal_begin()
{
    input_scale_corrections.begin(false);
    input_scale_corrections.xfer();
    input_offset_corrections.begin(false);
    input_offset_corrections.xfer();
    input_calibrated.begin(false);
    input_calibrated.xfer();

    input_cal_fxn.begin();
    input_cal_fxn.string_params = input_cal_string_params;
    input_cal_fxn.offsets = input_cal_offsets;
    input_cal_fxn.update_fxns = input_cal_update_fxns;
    input_cal_fxn.display_fxn = &input_cal_display;
}

bool in_input_cal_fxn()
{
    return selected_fxn == &input_cal_fxn;
}

// 1.65V * 3.01, A/D FS * circuit gain
#define IN_FS_VOLTS 4.9665

String check_input_cal()
{
    int param_num = input_cal_fxn.param_num;
    if (param_num == INPUT_CAL_OFFSET || param_num == INPUT_CAL_SCALE)
    {
        int input = input_cal_fxn.get_param(INPUT_CAL_INPUT_NUM);
        int val = adc_read(input);
        // int raw = adc_read_raw(input);
        // float reading = 3.23 * val / 1000.0;
        float reading = IN_FS_VOLTS * val / 1650;
        // return "Input: " + String(input) + " Reading: " + String(reading) + " Value: " + String(val) + " raw: " + String(raw);
        return "Input: " + String(input) + " Reading: " + String(reading);
    }
    else
    {
        return "Apply 0V to set Offset, 4V to set Scale";
    }
}

void input_cal_params_macro()
{
    Greenface_gadget *fxn = &input_cal_fxn;
    input_cal_fxn.param_put(0, INPUT_CAL_INPUT_NUM);
    gen_params_macro(fxn, true);
    input_cal_fxn.param_put(1, INPUT_CAL_INPUT_NUM);
    gen_params_macro(fxn, false);
}
