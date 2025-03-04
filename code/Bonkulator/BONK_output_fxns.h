#include <Greenface_gadget.h>

// pre-installed waveforms
#define WAVEFORM_SINE 0
#define WAVEFORM_HAYSTACK 1
#define WAVEFORM_RAMP 2
#define WAVEFORM_PYRAMID 3
#define WAVEFORM_PULSE 4
#define WAVEFORM_MAYTAG 5
#define WAVEFORM_TOGGLE 6
#define NUM_STD_WAVEFORMS 7
#define NUM_WAVEFORMS 15

#define INIT_STD_WAVEFORMS "Sine,Haystack,Ramp,Pyramid,Pulse,Maytag,Toggle"
#define INIT_WAVEFORMS "Sine,Haystack,Ramp,Pyramid,Pulse,Maytag,Toggle,User 0,User 1,User 2,User 3,User 4,User 5,User 6,User 7"

// used by timer.h
#define CV_OFF 0
#define CV_SCALE 1
#define CV_OFFSET 2
#define CV_ACTIVE_TIME 3
#define CV_IDLE_VALUE 4
#define CV_RANDOMNESS 5
#define CV_TRIGGER 6

#define CV_TYPES "Off,Scale,Offset,Active Time,Idle Value,Randomness,Trigger"

#define TIME_INC_MIN 1
#define TIME_INC_MAX 32000

#define ACTIVE_TIME_MIN 10
#define MAX_MS 65535
#define DELAY_MAX 32767

#define OUTPUT_SCALE_OFFSET -100
#define OUTPUT_OFFSET_OFFSET -100

#define OLD_OUTPUT_IDLE_VALUE_MAX 10956
#define OUTPUT_IDLE_VALUE_MAX 10666
#define OUTPUT_IDLE_VALUE_INIT 5478
#define OUTPUT_IDLE_VALUE_OFFSET -5478

#define TRIGGER_STATES F("Disabled,Enabled ")
void output_update_range();
void update_idle_value();
void update_waveform();

uint16_t _output_mins[] = {0, 0, ACTIVE_TIME_MIN, 0, 0, 0, 0, 0, 0, CV_OFF, CV_OFF, 0, 0, 0, 0, 0, 0, OUTPUT_RANGE_BIPOLAR, TRIG_HOLDOFF, 0};
uint16_t _output_maxs[] = {NUM_WAVEFORMS - 1, DELAY_MAX, MAX_MS, DELAY_MAX, 32767, 1, 1, 1, 1, CV_TRIGGER, CV_TRIGGER, 200, 200, 99, 1, OUTPUT_IDLE_VALUE_MAX, 1, OUTPUT_RANGE_UNIPOLAR, TRIG_DENSITY, MAX_MS};
uint16_t _output_init_vals[] = {0, 0, 128, 0, 0, 0, 0, 0, 0, CV_OFF, CV_OFF, 200, 100, 0, 0, OUTPUT_IDLE_VALUE_INIT, 0, OUTPUT_RANGE_BIPOLAR, TRIG_HOLDOFF, 5};
String output_labels[] = {"Waveform: ", "Init Delay: ", "Active Time/Parts: ", "Idle Time: ", "Repeat: ", "T0: ", "T1: ", "T2: ", "T3: ", "CV0: ", "CV1: ", "Scale: ", "Offset: ", "Randomness: ", "Quantize: ", "Idle Value: ", "Clock: ", "Range: ", "Trig Crtl: ", "Trig Holdoff: "};
String output_string_params[] = {INIT_WAVEFORMS, "", "", "", "", TRIGGER_STATES, TRIGGER_STATES, TRIGGER_STATES, TRIGGER_STATES, CV_TYPES, CV_TYPES, "", "", "", "No ,Yes", "", "Internal,External", "+/-5V,0-10V", "Hold_Off,Skip,Density", ""};
int8_t decimal_places[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0}; // allows fixed point numbers

// Output definitions
uint16_t _output0_params[NUM_OUTPUT_PARAMS];
uint16_t *output0_stuff[] = {_output0_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output0("Output 0", output_labels, output0_stuff, sizeof(_output0_params) / sizeof(_output0_params[0]));

uint16_t _output1_params[NUM_OUTPUT_PARAMS];
uint16_t *output1_stuff[] = {_output1_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output1("Output 1", output_labels, output1_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]));

uint16_t _output2_params[NUM_OUTPUT_PARAMS];
uint16_t *output2_stuff[] = {_output2_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output2("Output 2", output_labels, output2_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]));

uint16_t _output3_params[NUM_OUTPUT_PARAMS];
uint16_t *output3_stuff[] = {_output3_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output3("Output 3", output_labels, output3_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]));

uint16_t _output4_params[NUM_OUTPUT_PARAMS];
uint16_t *output4_stuff[] = {_output4_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output4("Output 4", output_labels, output4_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]));

uint16_t _output5_params[NUM_OUTPUT_PARAMS];
uint16_t *output5_stuff[] = {_output5_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output5("Output 5", output_labels, output5_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]));

uint16_t _output6_params[NUM_OUTPUT_PARAMS];
uint16_t *output6_stuff[] = {_output6_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output6("Output 6", output_labels, output6_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]));

uint16_t _output7_params[NUM_OUTPUT_PARAMS];
uint16_t *output7_stuff[] = {_output7_params, _output_mins, _output_maxs, _output_init_vals};
Greenface_gadget output7("Output 7", output_labels, output7_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]));

Greenface_gadget *bonk_outputs[NUM_OUTPUTS] = {
    &output0,
    &output1,
    &output2,
    &output3,
    &output4,
    &output5,
    &output6,
    &output7,
};

String alt_values0[NUM_OUTPUT_PARAMS] = {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};

String alt_values1[NUM_OUTPUT_PARAMS] = {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};

String alt_values2[NUM_OUTPUT_PARAMS] = {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};

String alt_values3[NUM_OUTPUT_PARAMS] = {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};

String alt_values4[NUM_OUTPUT_PARAMS] = {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};

String alt_values5[NUM_OUTPUT_PARAMS] = {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};

String alt_values6[NUM_OUTPUT_PARAMS] = {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};

String alt_values7[NUM_OUTPUT_PARAMS] = {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};

String *alt_values[NUM_OUTPUTS] =
    {
        alt_values0,
        alt_values1,
        alt_values2,
        alt_values3,
        alt_values4,
        alt_values5,
        alt_values6,
        alt_values7,
};

int16_t output_offsets0[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OUTPUT_SCALE_OFFSET, OUTPUT_OFFSET_OFFSET, 0, 0, OUTPUT_IDLE_VALUE_OFFSET, 0, 0, 0, 0}; // allows negative numbers
int16_t output_offsets1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OUTPUT_SCALE_OFFSET, OUTPUT_OFFSET_OFFSET, 0, 0, OUTPUT_IDLE_VALUE_OFFSET, 0, 0, 0, 0}; // allows negative numbers
int16_t output_offsets2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OUTPUT_SCALE_OFFSET, OUTPUT_OFFSET_OFFSET, 0, 0, OUTPUT_IDLE_VALUE_OFFSET, 0, 0, 0, 0}; // allows negative numbers
int16_t output_offsets3[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OUTPUT_SCALE_OFFSET, OUTPUT_OFFSET_OFFSET, 0, 0, OUTPUT_IDLE_VALUE_OFFSET, 0, 0, 0, 0}; // allows negative numbers
int16_t output_offsets4[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OUTPUT_SCALE_OFFSET, OUTPUT_OFFSET_OFFSET, 0, 0, OUTPUT_IDLE_VALUE_OFFSET, 0, 0, 0, 0}; // allows negative numbers
int16_t output_offsets5[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OUTPUT_SCALE_OFFSET, OUTPUT_OFFSET_OFFSET, 0, 0, OUTPUT_IDLE_VALUE_OFFSET, 0, 0, 0, 0}; // allows negative numbers
int16_t output_offsets6[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OUTPUT_SCALE_OFFSET, OUTPUT_OFFSET_OFFSET, 0, 0, OUTPUT_IDLE_VALUE_OFFSET, 0, 0, 0, 0}; // allows negative numbers
int16_t output_offsets7[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OUTPUT_SCALE_OFFSET, OUTPUT_OFFSET_OFFSET, 0, 0, OUTPUT_IDLE_VALUE_OFFSET, 0, 0, 0, 0}; // allows negative numbers

int16_t *output_offsets[NUM_OUTPUTS] =
    {
        output_offsets0,
        output_offsets1,
        output_offsets2,
        output_offsets3,
        output_offsets4,
        output_offsets5,
        output_offsets6,
        output_offsets7,
};

void update_clock();

Greenface_gadget bonk_output(int output_num)
{
    return *bonk_outputs[output_num];
}

Greenface_gadget the_output()
{
    return *bonk_outputs[selected_output.get()];
}

void clear_alt_values()
{
    for (int i = 0; i < NUM_OUTPUT_PARAMS; i++)
    {
        (the_output)().alt_values[i] = "";
    }
}

void set_cv_alt_values(int cv, String label)
{
    // CV_TYPES;
    int param = -1;
    switch (cv)
    {
    case 1:
        param = OUTPUT_SCALE;
        break;
    case 2:
        param = OUTPUT_OFFSET;
        break;
    case 3:
        param = OUTPUT_ACTIVE_TIME;
        break;
    case 4:
        param = OUTPUT_IDLE_VALUE;
        break;
    case 5:
        param = OUTPUT_RANDOMNESS;
        break;
    }
    if (param != -1)
    {
        if ((the_output)().alt_values[param] > "" && (the_output)().alt_values[param] != label)
        {
            (the_output)().alt_values[param] += ("/" + label);
        }
        else
        {
            (the_output)().alt_values[param] = label;
        }
    }
}

String output_get_name(int output_num)
{
    String text = (*bonk_outputs[output_num]).name;
    if (!get_output_calibrated(output_num))
        text += "*";
    return text;
}

bool is_toggle_waveform()
{
    int waveform = (the_output)().get_param(OUTPUT_WAVEFORM);
    return waveform == WAVEFORM_TOGGLE;
}

void output_display()
{
    update_clock();
    update_waveform();
    (the_output)().labels[OUTPUT_CV0] = get_input_calibrated(0) ? "CV0: " : "CV0* ";
    (the_output)().labels[OUTPUT_CV1] = get_input_calibrated(1) ? "CV1: " : "CV1* ";

    clear_alt_values();
    set_cv_alt_values((the_output)().get_param(OUTPUT_CV0), "CV0");
    set_cv_alt_values((the_output)().get_param(OUTPUT_CV1), "CV1");

    int wave_num = (the_output)().get_param(OUTPUT_WAVEFORM);
    clear_alt_values();
    switch (wave_num)
    {
    case WAVEFORM_TOGGLE:
        (the_output)().alt_values[OUTPUT_ACTIVE_TIME] = "N/A";
        (the_output)().alt_values[OUTPUT_IDLE_TIME] = "N/A";
        (the_output)().alt_values[OUTPUT_REPEAT] = "N/A";
        (the_output)().alt_values[OUTPUT_IDLE_VALUE] = "N/A";
        (the_output)().alt_values[OUTPUT_CLOCK] = "N/A";
        break;
    case WAVEFORM_PULSE:
        (the_output)().alt_values[OUTPUT_OFFSET] = "N/A";
        // (the_output)().labels[OUTPUT_ACTIVE_TIME] = "Active Time: ";
        // (the_output)().labels[OUTPUT_IDLE_TIME] = "Idle Time: ";
        break;
    default:
        // clear_alt_values();
        break;
    }

    output_update_range();
    update_idle_value();

    (the_output)().default_display();

    String text = output_get_name(selected_output.get());
    ui.printLine(text, 0, 2);
    // ui.terminal_debug("Waveform: " + String(wave_num));
    apply_params_to_waveform(selected_output.get());
    get_waveform_parts(selected_output.get());
    // ui.terminal_debug("Output: " + String(selected_output.get()));
}

void display_output()
{
    Greenface_gadget output = the_output();
    output.display();
    // (the_output)().display();
    // (*bonk_outputs[selected_output.get()]).display();
}

void init_all_outputs()
{
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        (*bonk_outputs[i]).init();
    }
}

bool output_is_bipolar(uint8_t output)
{
    return (*bonk_outputs[output]).get_param(OUTPUT_RANGE) == OUTPUT_RANGE_BIPOLAR;
}

String output_get_fs()
{
    // float ideal_fs = 128.0 / 24.0; // 5.33333V
    // // float ideal_fs = 5.478; // 3.3*3.32 / 2
    // uint16_t scale_correction = get_output_scale_correction(selected_output.get());
    // uint16_t ideal_dac = (scale_correction / 1000.0) * DAC_FS; // should produce ideal fs
    // float fs_volts = (DAC_FS - DAC_MID) * ideal_fs / (ideal_dac - DAC_MID);
    // return String(fs_volts * 2);
    int idle_max = settings_get_fs_fixed() ? OLD_OUTPUT_IDLE_VALUE_MAX : OUTPUT_IDLE_VALUE_MAX;
    return String(idle_max);
}

String output_get_fs_offset()
{
    uint16_t range = (the_output)().get_param(OUTPUT_RANGE);
    return String(range);
}

String list_outputs()
{
    String out = "";
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        if (i > 0)
        {
            out += ", ";
        }
        out += enquote(output_get_name(i));
    }
    return out;
}

bool any_output_triggered()
{
    bool any_triggered = false;
    int i = 0;
    while (!any_triggered && i < NUM_OUTPUTS)
    {
        any_triggered = (*bonk_outputs[i++]).triggered;
    }
    return any_triggered;
}

void add_waveform()
{
    _output_maxs[OUTPUT_WAVEFORM]++;
    output_string_params[OUTPUT_WAVEFORM] += ",User Wave 0";
}

void output_update_trigger(int output)
{
    int trig_num = bonk_outputs[output]->param_num - OUTPUT_ENABLE_T0;
    int trig_type = bonk_outputs[output]->get_param();
    // int trig_hold_off = bonk_outputs[output]->get_param(OUTPUT_TRIG_CTRL_VAL);
    // ui.terminal_debug("Selected output: " + String(output) + " trig num: " + String(trig_num) + " trig_type: " + String(trig_type));
    switch (trig_num)
    {
    case 0:
        trig0.set_channel(output, trig_type);
        break;
    case 1:
        trig1.set_channel(output, trig_type);
        break;
    case 2:
        trig2.set_channel(output, trig_type);
        break;
    case 3:
        trig3.set_channel(output, trig_type);
        break;
    }
}

void update_trigger()
{
    output_update_trigger(selected_output.get());
}

void update_user_waveform_names(String names)
{
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        Greenface_gadget *output = bonk_outputs[i];
        output->string_params[0] = String(INIT_STD_WAVEFORMS + names);
    }
}

void update_waveform()
{
    int wave_num = (the_output)().get_param(OUTPUT_WAVEFORM);
    set_waveform(selected_output.get(), wave_num);
}

void update_active_time()
{
    outputs[selected_output.get()].active_time = (the_output)().get_param(OUTPUT_ACTIVE_TIME);
    update_waveform();
}

void update_and_graph_waveform()
{
    // update_waveform();
    output_display();
    graph_waveform(selected_output.get());
}

void update_cv0()
{
    update_cv(OUTPUT_CV0);
}

void update_cv1()
{
    update_cv(OUTPUT_CV1);
}

void set_trig_ctrls(TRIGGER *trig)
{
    trig->set_trig_ctrls(selected_output.get(), (the_output)().get_param(OUTPUT_TRIG_CTRL), (the_output)().get_param(OUTPUT_TRIG_CTRL_VAL));
}

void update_trig_ctrl_vals()
{
    for (int i = 0; i < NUM_TRIGGERS; i++)
    {
        set_trig_ctrls(triggers[i]);
    }
    set_trig_ctrls(&trig_cv0);
    set_trig_ctrls(&trig_cv1);
}

void update_trig_ctrl()
{
    // Serial.println("update_trig_ctrl: " + String((the_output)().get_param(OUTPUT_TRIG_CTRL)));
    switch ((the_output)().get_param(OUTPUT_TRIG_CTRL))
    {
    case TRIG_HOLDOFF:
        (the_output)().labels[OUTPUT_TRIG_CTRL_VAL] = F("Trig Holdoff: ");
        (the_output)().maxs[OUTPUT_TRIG_CTRL_VAL] = MAX_MS;
        break;
    case TRIG_SKIP:
        (the_output)().labels[OUTPUT_TRIG_CTRL_VAL] = F("Trig Skip: ");
        (the_output)().maxs[OUTPUT_TRIG_CTRL_VAL] = MAX_MS;
        break;
    case TRIG_DENSITY:
        (the_output)().labels[OUTPUT_TRIG_CTRL_VAL] = F("Trig Density: ");
        (the_output)().maxs[OUTPUT_TRIG_CTRL_VAL] = 100; // 100%
        break;
    }
    // now set value in case the max was changed
    (the_output)().param_put((the_output)().get_param(OUTPUT_TRIG_CTRL_VAL), OUTPUT_TRIG_CTRL_VAL);
    update_trig_ctrl_vals();
}

// randomness range 0:100, scale range 0.0:1.0
void set_randomness_factor(uint16_t randomness, float scale_factor, int output)
{
    OutputData *outptr = &outputs[output];
    float randomness_factor = DAC_FS * scale_factor * randomness / 200;
    outptr->randomness_factor = (int)randomness_factor;
}

void update_randomness_factor()
{
    int output = selected_output.get();
    float scale_factor = (the_output)().get_param(OUTPUT_SCALE) / 100.0;
    int randomness = (the_output)().get_param(OUTPUT_RANDOMNESS);
    set_randomness_factor(randomness, scale_factor, output);
}

void set_idle_value(int val, int output)
{
    OutputData *outptr = &outputs[output];
    outptr->idle_value = max(0, val);
}

void send_idle_value(int output_num)
{
    dac_out(output_num, outputs[output_num].idle_value);
}

int scale_dac(float volts, int output)
{
    int idle_max = settings_get_fs_fixed() ? OLD_OUTPUT_IDLE_VALUE_MAX : OUTPUT_IDLE_VALUE_MAX;
    float fs = (float)idle_max / 1000.0;
    if (output_is_bipolar(output))
    {
        volts += fs / 2.0;
    }
    float portion = volts / fs;
    int dac_val = int((float)DAC_FS * portion);
    // if (settings_get_fs_fixed())
    // {
    //     dac_val *= (get_output_scale_correction(output) / 1000.0);
    // }
    dac_val += get_raw_output_offset_correction(output);
    ui.terminal_debug("DAC: " + String(dac_val) + " is_bipolar: " + String(output_is_bipolar(output)) + " volts: " + String(volts));
    return dac_val;
}

void dac_vdc(float volts, int output)
{
    dac_out(output, scale_dac(volts, output));
}

void update_idle_value()
{
    int output = selected_output.get();
    float val = (float)(*bonk_outputs[output]).get_param_w_offset(OUTPUT_IDLE_VALUE) / 1000.0;
    set_idle_value(scale_dac(val, output), output);
    if (!(the_output)().triggered)
        send_idle_value(output);
}

void update_clock()
{
    switch ((the_output)().get_param(OUTPUT_CLOCK))
    {
    case 1:
        (the_output)().labels[OUTPUT_ACTIVE_TIME] = "Parts: ";
        (the_output)().maxs[OUTPUT_ACTIVE_TIME] = WAVEFORM_PARTS;
        (the_output)().alt_values[OUTPUT_INIT_DELAY] = "N/A";
        (the_output)().alt_values[OUTPUT_IDLE_TIME] = "N/A";
        (the_output)().alt_values[OUTPUT_REPEAT] = "N/A";
        break;
    case 0:
        (the_output)().labels[OUTPUT_ACTIVE_TIME] = "Active Time: ";
        (the_output)().maxs[OUTPUT_ACTIVE_TIME] = MAX_MS;
        (the_output)().alt_values[OUTPUT_INIT_DELAY] = "";
        (the_output)().alt_values[OUTPUT_IDLE_TIME] = "";
        (the_output)().alt_values[OUTPUT_REPEAT] = "";
        break;
    }
}

void output_update_range()
{
    int output = selected_output.get();
    uint16_t range_type = (the_output)().get_param(OUTPUT_RANGE);
    set_output_range(output, (output_range_type)range_type);
    (the_output)().offsets[OUTPUT_IDLE_VALUE] = (the_output)().get_param(OUTPUT_RANGE) == OUTPUT_RANGE_UNIPOLAR ? 0 : OUTPUT_IDLE_VALUE_OFFSET;
}

update_fxn output_update_fxns[NUM_OUTPUT_PARAMS] = {
    update_and_graph_waveform,
    nullptr,
    update_active_time,
    nullptr,
    nullptr,
    update_trigger,
    update_trigger,
    update_trigger,
    update_trigger,
    update_cv0,
    update_cv1,
    update_waveform,
    update_waveform,
    update_waveform,
    nullptr,
    update_idle_value,
    update_clock,
    output_update_range,
    update_trig_ctrl,
    update_trig_ctrl_vals};

void outputs_begin()
{
    int output_memory = selected_output.get();
    Greenface_gadget *selected_fxn_memory = selected_fxn;
    if (settings_get_fs_fixed())
    {
        _output_maxs[OUTPUT_IDLE_VALUE] = OLD_OUTPUT_IDLE_VALUE_MAX;
    }
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        Greenface_gadget *output = bonk_outputs[i];
        output->begin();
        output->string_params = output_string_params;
        output->update_fxns = output_update_fxns;
        output->offsets = output_offsets[i];
        output->decimal_places = decimal_places;
        output->display_fxn = &output_display;
        output->alt_values = alt_values[i];
        outputs[i].active_time = output->get_param(OUTPUT_ACTIVE_TIME);

        selected_output.put(i); // required by update_ fxns
        selected_fxn = bonk_outputs[i];
        update_idle_value();
        update_trig_ctrl();
        update_cv0();
        update_cv1();
        update_clock();
        if (output_ranges_installed)
        {
            output_update_range();
        }
        else
        {
            output->maxs[OUTPUT_RANGE] = (uint16_t)OUTPUT_RANGE_BIPOLAR;
        }
        // update_repeat_count();
        for (int j = 0; j < 4; j++)
        {
            output->param_num = OUTPUT_ENABLE_T0 + j;
            update_trigger();
        }
        output->param_num = OUTPUT_WAVEFORM;
    }
    selected_fxn = selected_fxn_memory;
    selected_output.put(output_memory);
    // update_clock();
    // update_trig_ctrl();
}

void check_cv_inputs()
{
    bool adc0_in_use = false;
    bool adc1_in_use = false;
    for (int output = 0; output < NUM_OUTPUTS; output++)
    {
        if ((*bonk_outputs[output]).get_param(OUTPUT_CV0) > 0)
            adc0_in_use = true;
        if ((*bonk_outputs[output]).get_param(OUTPUT_CV1) > 0)
            adc1_in_use = true;
    }
    // ui.terminal_debug("adc0_in_use: " + String(adc0_in_use) + " adc1_in_use: " + String(adc1_in_use));
    if (adc0_in_use || adc1_in_use)
    {
        if (adc0_in_use)
        {
            adc0 = adc_read(0);
        }
        if (adc1_in_use)
        {
            adc1 = adc_read(1);
        }

        for (int cv = 0; cv < 2; cv++)
        {
            for (int output = 0; output < NUM_OUTPUTS; output++)
            {
                if (adc0_in_use)
                    cv_set(OUTPUT_CV0, output, adc0);
                if (adc1_in_use)
                    cv_set(OUTPUT_CV1, output, adc1);
            }
        }
    }
}

String check_outputs()
{
    bool output_in_use;
    for (int output = 0; output < NUM_OUTPUTS; output++)
    {
        output_in_use = false;
        for (int trig = 0; trig < 4; trig++)
        {
            if ((*bonk_outputs[output]).get_param(OUTPUT_ENABLE_T0 + trig) > 0 && triggers[trig]->state == TRIGGER_ACTIVE)
            {
                output_in_use = true;
            }
        }
        if (!output_in_use)
        {
            (*bonk_outputs[output]).triggered = false;
        }
    }
    String out = "";
    String t = "     ";
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        if ((*bonk_outputs[i]).get_param(OUTPUT_CLOCK) == 1)
        {
            t = " X" + String(bonk_output(i).triggered_by) + "  ";
        }
        else
        {
            t = "     ";
            if ((*bonk_outputs[i]).triggered)
            {
                t = " T" + String(bonk_output(i).triggered_by) + "  ";
            }
        }
        out += String(i) + ":" + t;
    }
    return out;
}
