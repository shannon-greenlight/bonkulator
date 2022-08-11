#include "SPANK_fxn.h"

enum
{
    OUTPUT_WAVEFORM,
    OUTPUT_PERIOD,
    OUTPUT_INIT_DELAY,
    OUTPUT_POST_DELAY,
    OUTPUT_REPEAT,
    OUTPUT_ENABLE_T0,
    OUTPUT_ENABLE_T1,
    OUTPUT_ENABLE_T2,
    OUTPUT_ENABLE_T3,
    OUTPUT_CV0,
    OUTPUT_CV1,
    OUTPUT_SCALE,
    OUTPUT_OFFSET,
    OUTPUT_RANDOMNESS,
    OUTPUT_QUANTIZE,
    OUTPUT_IDLE_VALUE,
    OUTPUT_CLOCK,
    NUM_OUTPUT_PARAMS
};

// pre-installed waveforms
#define WAVEFORM_SINE 0
#define WAVEFORM_INV_SINE 1
#define WAVEFORM_RAMP_UP 2
#define WAVEFORM_RAMP_DN 3
#define WAVEFORM_PULSE 4
#define WAVEFORM_INV_PULSE 5
#define WAVEFORM_TOGGLE 6
#define NUM_STD_WAVEFORMS 7
#define NUM_WAVEFORMS 15

#define INIT_STD_WAVEFORMS "Sine,Inv Sine,Ramp Up,Ramp Down,Pulse,Inv Pulse,Toggle"
#define INIT_WAVEFORMS "Sine,Inv Sine,Ramp Up,Ramp Down,Pulse,Inv Pulse,Toggle,User 0,User 1,User 2,User 3,User 4,User 5,User 6,User 7"

// used by timer.h
#define CV_OFF 0
#define CV_SCALE 1
#define CV_OFFSET 2
#define CV_PERIOD 3

#define TIME_INC_MIN 1
#define TIME_INC_MAX 32000

#define PERIOD_MIN 10
#define PERIOD_MAX 32767
#define DELAY_MAX 32767

#define OUTPUT_OFFSET_OFFSET -100

#define TRIGGER_STATES F("Disabled,Enabled ")

uint16_t _output_mins[] = {0, PERIOD_MIN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t _output_maxs[] = {NUM_WAVEFORMS - 1, PERIOD_MAX, DELAY_MAX, DELAY_MAX, 32767, 1, 1, 1, 1, 3, 3, 100, 200, 99, 1, DAC_FS, 1};
uint16_t _output_init_vals[] = {0, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 0, 0, 512, 0};
String output_labels[] = {"Waveform: ", "Period/Parts: ", "Init Delay: ", "Post Delay: ", "Repeat: ", "T0: ", "T1: ", "T2: ", "T3: ", "CV0: ", "CV1: ", "Scale: ", "Offset: ", "Randomness: ", "Quantize: ", "Idle Value: ", "Clock: "};
String output_string_params[] = {INIT_WAVEFORMS, "", "", "", "", TRIGGER_STATES, TRIGGER_STATES, TRIGGER_STATES, TRIGGER_STATES, "Off   ,Scale ,Offset,Period", "Off   ,Scale ,Offset,Time-inc", "", "", "", "No ,Yes", "", "Internal,External"};
int16_t output_offsets[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OUTPUT_OFFSET_OFFSET, 0, 0, 0}; // allows negative numbers

// Output definitions
uint16_t _output0_params[NUM_OUTPUT_PARAMS];
uint16_t *output0_stuff[] = {_output0_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output0("Output 0", output_labels, output0_stuff, sizeof(_output0_params) / sizeof(_output0_params[0]), &ui);

uint16_t _output1_params[NUM_OUTPUT_PARAMS];
uint16_t *output1_stuff[] = {_output1_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output1("Output 1", output_labels, output1_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]), &ui);

uint16_t _output2_params[NUM_OUTPUT_PARAMS];
uint16_t *output2_stuff[] = {_output2_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output2("Output 2", output_labels, output2_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]), &ui);

uint16_t _output3_params[NUM_OUTPUT_PARAMS];
uint16_t *output3_stuff[] = {_output3_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output3("Output 3", output_labels, output3_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]), &ui);

uint16_t _output4_params[NUM_OUTPUT_PARAMS];
uint16_t *output4_stuff[] = {_output4_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output4("Output 4", output_labels, output4_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]), &ui);

uint16_t _output5_params[NUM_OUTPUT_PARAMS];
uint16_t *output5_stuff[] = {_output5_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output5("Output 5", output_labels, output5_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]), &ui);

uint16_t _output6_params[NUM_OUTPUT_PARAMS];
uint16_t *output6_stuff[] = {_output6_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output6("Output 6", output_labels, output6_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]), &ui);

uint16_t _output7_params[NUM_OUTPUT_PARAMS];
uint16_t *output7_stuff[] = {_output7_params, _output_mins, _output_maxs, _output_init_vals};
SPANK_fxn output7("Output 7", output_labels, output7_stuff, sizeof(_output1_params) / sizeof(_output1_params[0]), &ui);

SPANK_fxn *bonk_outputs[NUM_OUTPUTS] = {
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

void update_clock();

SPANK_fxn bonk_output(int output_num)
{
    return *bonk_outputs[output_num];
}

SPANK_fxn the_output()
{
    return *bonk_outputs[selected_output.get()];
}

void output_display()
{
    update_clock();
    (the_output)().labels[OUTPUT_CV0] = get_input_calibrated(0) ? "CV0: " : "CV0* ";
    (the_output)().labels[OUTPUT_CV1] = get_input_calibrated(1) ? "CV1: " : "CV1* ";

    int wave_num = (the_output)().get_param(OUTPUT_WAVEFORM);
    switch (wave_num)
    {
    case WAVEFORM_TOGGLE:
        (the_output)().alt_values[OUTPUT_PERIOD] = "N/A";
        (the_output)().alt_values[OUTPUT_POST_DELAY] = "N/A";
        (the_output)().alt_values[OUTPUT_REPEAT] = "N/A";
        break;
    default:
        // update_clock();
        (the_output)().alt_values[OUTPUT_PERIOD] = "";
        (the_output)().alt_values[OUTPUT_POST_DELAY] = "";
        (the_output)().alt_values[OUTPUT_REPEAT] = "";
        break;
    }

    (the_output)().default_display();
    String text = (the_output)().name;

    if (!get_output_calibrated(selected_output.get()))
        text += "*";
    ui.printLine(text, 0, 2);
}

void display_output()
{
    SPANK_fxn output = the_output();
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

String output_get_fs()
{
    float ideal_fs = 128.0 / 24.0; // 5.33333V
    uint16_t scale_correction = get_output_scale_correction(selected_output.get());
    uint16_t ideal_dac = (scale_correction / 10000.0) * DAC_FS; // should produce ideal fs
    float fs_volts = (DAC_FS - DAC_MID) * ideal_fs / (ideal_dac - DAC_MID);
    return String(fs_volts * 2);
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
        out += enquote((*bonk_outputs[i]).name);
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

void reset_outputs(int trig_num)
{
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        if ((*bonk_outputs[i]).get_param(OUTPUT_ENABLE_T0 + trig_num))
        {
            (*bonk_outputs[i]).triggered = false;
        }
    }
}

void add_waveform()
{
    _output_maxs[OUTPUT_WAVEFORM]++;
    output_string_params[OUTPUT_WAVEFORM] += ",User Wave 0";
}

void output_update_trigger()
{
    // SPANK_fxn output = *bonk_outputs[selected_output.get()];
    int trig_num = (the_output)().param_num - OUTPUT_ENABLE_T0;
    int trig_type = (the_output)().get_param();
    // Serial.println("Selected output: " + String(selected_output.get()) + " trigger: " + String(trig_num) + " trig_type: " + String(trig_type));
    switch (trig_num)
    {
    case 0:
        trig0.set_output(selected_output.get(), trig_type);
        break;
    case 1:
        trig1.set_output(selected_output.get(), trig_type);
        break;
    case 2:
        trig2.set_output(selected_output.get(), trig_type);
        break;
    case 3:
        trig3.set_output(selected_output.get(), trig_type);
        break;
    }
}

void update_user_waveform_names(String names)
{
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        SPANK_fxn *output = bonk_outputs[i];
        output->string_params[0] = String(INIT_STD_WAVEFORMS + names);
    }
}

void update_waveform()
{
    int wave_num = (the_output)().get_param(OUTPUT_WAVEFORM);
    set_waveform(selected_output.get(), wave_num);
}

void update_and_graph_waveform()
{
    update_waveform();
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

void update_idle_value()
{
    // ui.terminal_debug("update_idle_value: " + String((the_output)().triggered));
    if (!(the_output)().triggered)
        set_idle_value(selected_output.get());
}

void set_idle_value(int output_num)
{
    dac_out(output_num, (*bonk_outputs[output_num]).get_param(OUTPUT_IDLE_VALUE) + get_raw_output_offset_correction(output_num));
}

void update_clock()
{
    switch ((the_output)().get_param(OUTPUT_CLOCK))
    {
    case 1:
        (the_output)().labels[OUTPUT_PERIOD] = "Parts: ";
        (the_output)().maxs[OUTPUT_PERIOD] = WAVEFORM_PARTS;
        (the_output)().alt_values[OUTPUT_INIT_DELAY] = "N/A";
        (the_output)().alt_values[OUTPUT_POST_DELAY] = "N/A";
        (the_output)().alt_values[OUTPUT_REPEAT] = "N/A";
        break;
    case 0:
        (the_output)().labels[OUTPUT_PERIOD] = "Period: ";
        (the_output)().maxs[OUTPUT_PERIOD] = PERIOD_MAX;
        (the_output)().alt_values[OUTPUT_INIT_DELAY] = "";
        (the_output)().alt_values[OUTPUT_POST_DELAY] = "";
        (the_output)().alt_values[OUTPUT_REPEAT] = "";
        break;
    }
}

update_fxn output_update_fxns[NUM_OUTPUT_PARAMS] = {
    update_and_graph_waveform,
    update_waveform,
    nullptr,
    nullptr,
    nullptr,
    output_update_trigger,
    output_update_trigger,
    output_update_trigger,
    output_update_trigger,
    update_cv0,
    update_cv1,
    update_waveform,
    update_waveform,
    update_waveform,
    nullptr,
    update_idle_value,
    update_clock};

void outputs_begin()
{
    int output_memory = selected_output.get();
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        SPANK_fxn *output = bonk_outputs[i];
        output->begin();
        output->string_params = output_string_params;
        output->update_fxns = output_update_fxns;
        output->offsets = output_offsets;
        output->display_fxn = &output_display;
        output->alt_values = alt_values[i];
        set_idle_value(i);

        selected_output.put(i); // required by update_ fxns
        update_clock();
        // update_repeat_count();
        for (int j = 0; j < 4; j++)
        {
            output->param_num = OUTPUT_ENABLE_T0 + j;
            output_update_trigger();
        }
        output->param_num = 0;
        set_waveform(i, output->get_param(OUTPUT_WAVEFORM));
    }
    selected_output.put(output_memory);
}

void check_outputs()
{
    bool in_use;
    for (int trig = 0; trig < 4; trig++)
    {
        in_use = false;
        for (int output = 0; output < NUM_OUTPUTS; output++)
        {
            bool this_trig_enabled;
            switch (trig)
            {
            case 0:
                this_trig_enabled = (*bonk_outputs[output]).get_param(OUTPUT_ENABLE_T0);
                break;
            case 1:
                this_trig_enabled = (*bonk_outputs[output]).get_param(OUTPUT_ENABLE_T1);
                break;
            case 2:
                this_trig_enabled = (*bonk_outputs[output]).get_param(OUTPUT_ENABLE_T2);
                break;
            case 3:
                this_trig_enabled = (*bonk_outputs[output]).get_param(OUTPUT_ENABLE_T3);
                break;
            }
            if (this_trig_enabled && (*bonk_outputs[output]).triggered)
            {
                // Serial.println((*bonk_outputs[output]).name + " triggered by: " + String(trig));
                in_use = true;
            }
        }
        if (!in_use)
        {
            // Serial.println("Turning off LED " + String(trig));
            switch (trig)
            {
            case 0:
                digitalWrite(T0_LED, LOW);
                reset_outputs(0);
                break;
            case 1:
                digitalWrite(T1_LED, LOW);
                reset_outputs(1);
                break;
            case 2:
                digitalWrite(T2_LED, LOW);
                reset_outputs(2);
                break;
            case 3:
                digitalWrite(T3_LED, LOW);
                reset_outputs(3);
                break;
            }
        }
    }

    adc0 = adc_read(0);
    adc1 = adc_read(1);

    for (int cv = 0; cv < 2; cv++)
    {
        for (int output = 0; output < NUM_OUTPUTS; output++)
        {
            cv_set(OUTPUT_CV0, output, adc0);
            cv_set(OUTPUT_CV1, output, adc1);
        }
    }
}
