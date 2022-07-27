#include "SPANK_fxn.h"

#define OUTPUT_WAVEFORM 0
#define OUTPUT_TIME_INC 1
#define OUTPUT_PERIOD 1
#define OUTPUT_DELAY 2
#define OUTPUT_REPEAT 3
#define OUTPUT_ENABLE_T0 4
#define OUTPUT_ENABLE_T1 5
#define OUTPUT_ENABLE_T2 6
#define OUTPUT_ENABLE_T3 7
#define OUTPUT_CV0 8
#define OUTPUT_CV1 9
#define OUTPUT_SCALE 10
#define OUTPUT_OFFSET 11
#define OUTPUT_RANDOMNESS 12
#define OUTPUT_QUANTIZE 13
#define OUTPUT_DACVAL 14
#define NUM_OUTPUT_PARAMS 15

// pre-installed waveforms
#define WAVEFORM_SINE 0
#define WAVEFORM_INV_SINE 1
#define WAVEFORM_RAMP_UP 2
#define WAVEFORM_RAMP_DN 3
#define WAVEFORM_PULSE 4
#define WAVEFORM_INV_PULSE 5
#define NUM_STD_WAVEFORMS 6
#define NUM_WAVEFORMS 14

#define INIT_STD_WAVEFORMS "Sine,Inv Sine,Ramp Up,Ramp Down,Pulse,Inv Pulse"
#define INIT_WAVEFORMS "Sine,Inv Sine,Ramp Up,Ramp Down,Pulse,Inv Pulse,User 0,User 1,User 2,User 3,User 4,User 5,User 6,User 7"

// used by timer.h
#define CV_OFF 0
#define CV_SCALE 1
#define CV_OFFSET 2
#define CV_TIME_INC 3
#define CV_PERIOD 3

#define TIME_INC_MIN 1
#define TIME_INC_MAX 32000

#define PERIOD_MIN 10
#define PERIOD_MAX 32000

#define OUTPUT_OFFSET_OFFSET -100

uint16_t _output_mins[] = {0, PERIOD_MIN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t _output_maxs[] = {NUM_WAVEFORMS - 1, PERIOD_MAX, 32000, 0xffff, 1, 1, 1, 1, 3, 3, 100, 200, 99, 1, DAC_FS};
uint16_t _output_init_vals[] = {0, 10, 10, 1, 0, 0, 0, 0, 0, 0, 100, 100, 0, 0, 512};
String output_labels[] = {"Waveform: ", "Period: ", "Delay: ", "Repeat: ", "T0: ", "T1: ", "T2: ", "T3: ", "CV0: ", "CV1: ", "Scale: ", "Offset: ", "Randomness: ", "Quantize: ", "Idle Value: "};
String output_string_params[] = {INIT_WAVEFORMS, "", "", "", "Disabled,Enabled ", "Disabled,Enabled ", "Disabled,Enabled ", "Disabled,Enabled ", "Off   ,Scale ,Offset,Period", "Off   ,Scale ,Offset,Time-inc", "", "", "", "No ,Yes", ""};
int16_t output_offsets[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OUTPUT_OFFSET_OFFSET, 0, 0, 0}; // allows negative numbers

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

SPANK_fxn bonk_output(int output_num)
{
    return *bonk_outputs[output_num];
}

SPANK_fxn the_output()
{
    return *bonk_outputs[selected_output.get()];
}

void amend_output_name()
{
    (the_output)().default_display();
    String text = (the_output)().name;
    bool calibrated = get_output_calibrated(selected_output.get());
    if (!calibrated)
        text += "*";
    ui.printLine(text, 0, 2);
    // (the_output)().printParams();
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

// String check_output_triggers()
// {
//     String out = "";
//     for (int i = 0; i < NUM_OUTPUTS; i++)
//     {
//         uint8_t trig_num = bonk_output(i).triggered_by;
//         String t = " T" + String(trig_num) + "  ";
//         out += String(i) + ":" + (bonk_output(i).triggered ? t : "     ");
//     }
//     return out;
// }

String output_get_fs()
{
    float ideal_fs = 128.0 / 24.0; // 5.33333V
    uint16_t scale_correction = get_scale_correction(selected_output.get());
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
    int val = (the_output)().get_param();
    // Serial.println("Selected output: " + String(selected_output.get()) + " trigger: " + String(trig_num) + " val: " + String(val));
    switch (trig_num)
    {
    case 0:
        trig0.set_output(selected_output.get(), val);
        break;
    case 1:
        trig1.set_output(selected_output.get(), val);
        break;
    case 2:
        trig2.set_output(selected_output.get(), val);
        break;
    case 3:
        trig3.set_output(selected_output.get(), val);
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

update_fxn output_update_fxns[NUM_OUTPUT_PARAMS] = {update_and_graph_waveform, update_waveform, nullptr, nullptr, output_update_trigger, output_update_trigger, output_update_trigger, output_update_trigger, update_cv0, update_cv1, update_waveform, update_waveform, update_waveform, nullptr};

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
        output->display_fxn = &amend_output_name;

        selected_output.put(i); // required by update_trigger()
        for (int j = 0; j < 4; j++)
        {
            output->param_num = OUTPUT_ENABLE_T0 + j;
            output_update_trigger();
        }
        output->param_num = 0;
        set_waveform(i, output->get_param(OUTPUT_WAVEFORM));
        // apply_params();
    }
    selected_output.put(output_memory);
}

void trigger_report()
{
    ui.t.clrDown("15");
    ui.terminal_printRow("Trigger Report", "16");
    Serial.println(F("Output\tT0\tT1\tT2\tT3\tTRIG"));
    ui.t.printChars(48, "-");
    Serial.print("\r\n");
    // Serial.println(F("----------------------------------"));
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        Serial.print(String(i));
        Serial.print("\t");
        Serial.print(String((*bonk_outputs[i]).get_param(OUTPUT_ENABLE_T0)));
        Serial.print("\t");
        Serial.print(String((*bonk_outputs[i]).get_param(OUTPUT_ENABLE_T1)));
        Serial.print("\t");
        Serial.print(String((*bonk_outputs[i]).get_param(OUTPUT_ENABLE_T2)));
        Serial.print("\t");
        Serial.print(String((*bonk_outputs[i]).get_param(OUTPUT_ENABLE_T3)));
        Serial.print("\t");
        Serial.println(String((*bonk_outputs[i]).triggered));
    }
}
