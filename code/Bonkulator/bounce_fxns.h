#include <Wire.h>

enum
{
    BOUNCE_OUTPUT,
    BOUNCE_REPEAT,
    BOUNCE_ENABLE_T0,
    BOUNCE_ENABLE_T1,
    BOUNCE_ENABLE_T2,
    BOUNCE_ENABLE_T3,
    BOUNCE_SCALE,
    BOUNCE_OFFSET,
    BOUNCE_SAMPLE_TIME,
    BOUNCE_TRIG_CTRL,
    BOUNCE_TRIG_CTRL_VAL,
    BOUNCE_NUM_PARAMS,
};

#define OUTPUT_CV_TYPES "Off   ,Scale ,Offset,Sample Time,Trigger"

uint16_t _bounce_mins[] = {0, 0, 0, 0, 0, 0, 0, 0, 100, TRIG_HOLDOFF, 0};
uint16_t _bounce_maxs[] = {7, 1, 1, 1, 1, 1, 200, 200, 32767, TRIG_DENSITY, MAX_MS};
uint16_t _bounce_init_vals[] = {0, 0, 0, 0, 0, 0, 200, 100, 100, TRIG_HOLDOFF, 5};
String bounce_string_params[] = {"", "No, Yes", TRIGGER_STATES, TRIGGER_STATES, TRIGGER_STATES, TRIGGER_STATES, "", "", "", "Hold_Off,Skip,Density", ""};
// int16_t bounce_offsets[] = {0, BOUNCE_OFFSET_CORRECTION, 0}; // allows negative numbers
// bool bounce_param_active[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

uint16_t _bounce0_params[BOUNCE_NUM_PARAMS];
uint16_t *bounce0_stuff[] = {_bounce0_params, _bounce_mins, _bounce_maxs, _bounce_init_vals};
String bounce0_labels[] = {"Output: ", "Repeat: ", "T0: ", "T1: ", "T2: ", "T3: ", "Scale: ", "Offset: ", "Sample Time: ", "Trig Crtl: ", "Trig Holdoff: "};
Greenface_gadget bounce0_fxn("Bounce 0", bounce0_labels, bounce0_stuff, sizeof(_bounce0_params) / sizeof(_bounce0_params[0]));

uint16_t _bounce1_params[BOUNCE_NUM_PARAMS];
uint16_t *bounce1_stuff[] = {_bounce1_params, _bounce_mins, _bounce_maxs, _bounce_init_vals};
String bounce1_labels[] = {"Output: ", "Repeat: ", "T0: ", "T1: ", "T2: ", "T3: ", "Scale: ", "Offset: ", "Sample Time: ", "Trig Crtl: ", "Trig Holdoff: "};
Greenface_gadget bounce1_fxn("Bounce 1", bounce1_labels, bounce1_stuff, sizeof(_bounce1_params) / sizeof(_bounce1_params[0]));

int16_t intput_offsets0[] = {0, 0, 0, 0, 0, 0, OUTPUT_SCALE_OFFSET, OUTPUT_OFFSET_OFFSET, 0, 0, 0}; // allows negative numbers
int16_t intput_offsets1[] = {0, 0, 0, 0, 0, 0, OUTPUT_SCALE_OFFSET, OUTPUT_OFFSET_OFFSET, 0, 0, 0}; // allows negative numbers

int16_t *input_offsets[NUM_INPUTS] =
    {
        intput_offsets0,
        intput_offsets1,
};

Greenface_gadget *bounce_inputs[NUM_INPUTS] = {
    &bounce0_fxn,
    &bounce1_fxn,
};

byte bounce_triggered_by[] = {0, 0};
bool bounce_triggered = false;
uint bounce_next_time;
int bounce_reading;
int bounce_debug;

int bounce_input()
{
    return selected_fxn == &bounce0_fxn ? 0 : 1;
}

void bounce_init()
{
    for (int i = 0; i < NUM_INPUTS; i++)
    {
        (*bounce_inputs[i]).init();
    }
    // bounce0_fxn.init();
    // bounce1_fxn.init();
}

bool in_bounce_fxn()
{
    return selected_fxn == &bounce0_fxn || selected_fxn == &bounce1_fxn;
}

void bounce_set_trig_ctrls(TRIGGER *trig)
{
    // Serial.println("bounce_set_trig_ctrls: " + String(bounce_input()));
    trig->set_trig_ctrls(bounce_input() + 8, selected_fxn->get_param(BOUNCE_TRIG_CTRL), selected_fxn->get_param(BOUNCE_TRIG_CTRL_VAL));
}

void bounce_update_trig_ctrl_vals()
{
    for (int i = 0; i < NUM_TRIGGERS; i++)
    {
        bounce_set_trig_ctrls(triggers[i]);
    }
    // bounce_set_trig_ctrls(&trig_cv0);
    // bounce_set_trig_ctrls(&trig_cv1);
}

void bounce_update_trig_ctrl()
{
    // Serial.println("update_trig_ctrl: " + String((the_output)().get_param(BOUNCE_TRIG_CTRL)));
    switch (selected_fxn->get_param(BOUNCE_TRIG_CTRL))
    {
    case TRIG_HOLDOFF:
        selected_fxn->labels[BOUNCE_TRIG_CTRL_VAL] = F("Trig Holdoff: ");
        selected_fxn->maxs[BOUNCE_TRIG_CTRL_VAL] = MAX_MS;
        break;
    case TRIG_SKIP:
        selected_fxn->labels[BOUNCE_TRIG_CTRL_VAL] = F("Trig Skip: ");
        selected_fxn->maxs[BOUNCE_TRIG_CTRL_VAL] = MAX_MS;
        break;
    case TRIG_DENSITY:
        selected_fxn->labels[BOUNCE_TRIG_CTRL_VAL] = F("Trig Density: ");
        selected_fxn->maxs[BOUNCE_TRIG_CTRL_VAL] = 100; // 100%
        break;
    }
    // now set value in case the max was changed
    selected_fxn->param_put(selected_fxn->get_param(BOUNCE_TRIG_CTRL_VAL), BOUNCE_TRIG_CTRL_VAL);
    bounce_update_trig_ctrl_vals();
}

void bounce_update_trigger()
{
    int input = selected_fxn == &bounce0_fxn ? 0 : 1;
    int channel = input + 8;
    int trig_num = selected_fxn->param_num - BOUNCE_ENABLE_T0;
    int trig_type = selected_fxn->get_param(); // enabled or disabled?
    // Serial.println("Selected input: " + String(input) + " trig num: " + String(trig_num) + " trig_type: " + String(trig_type));
    switch (trig_num)
    {
    case 0:
        trig0.set_channel(channel, trig_type);
        break;
    case 1:
        trig1.set_channel(channel, trig_type);
        break;
    case 2:
        trig2.set_channel(channel, trig_type);
        break;
    case 3:
        trig3.set_channel(channel, trig_type);
        break;
    }
}

update_fxn bounce_update_fxns[BOUNCE_NUM_PARAMS] = {
    nullptr,
    nullptr,
    bounce_update_trigger,
    bounce_update_trigger,
    bounce_update_trigger,
    bounce_update_trigger,
    nullptr,
    nullptr,
    nullptr,
    bounce_update_trig_ctrl,
    bounce_update_trig_ctrl_vals,
};

void bounce_begin()
{
    for (int i = 0; i < 2; i++)
    {
        (*bounce_inputs[i]).begin();
        (*bounce_inputs[i]).string_params = bounce_string_params;
        (*bounce_inputs[i]).update_fxns = bounce_update_fxns;
        (*bounce_inputs[i]).offsets = input_offsets[i];
        // (*bounce_inputs[i]).active_params = bounce_param_active;
        selected_fxn = bounce_inputs[i];
        for (int j = 0; j < NUM_TRIGGERS; j++)
        {
            selected_fxn->param_num = BOUNCE_ENABLE_T0 + j;
            bounce_update_trigger();
        }
    }

    // bounce0_fxn.begin();
    // bounce0_fxn.string_params = bounce_string_params;
    // bounce0_fxn.update_fxns = bounce_update_fxns;

    // bounce1_fxn.begin();
    // bounce1_fxn.string_params = bounce_string_params;
    // bounce1_fxn.update_fxns = bounce_update_fxns;

    // bounce_fxn.offsets = bounce_offsets;
    // bounce_fxn.update_fxns = bounce_update_fxns;
    // bounce_fxn.display_fxn = &bounce_display;
}

void reset_inputs(int trig_num)
{
    (*triggers[trig_num]).state = TRIGGER_IDLE;
    for (int i = 0; i < NUM_INPUTS; i++)
    {
        if ((*bounce_inputs[i]).get_param(BOUNCE_ENABLE_T0 + trig_num))
        {
            (*bounce_inputs[i]).triggered = false;
        }
    }
}

void check_bounce_triggers()
{
    bool in_use;
    for (int trig = 0; trig < 4; trig++)
    {
        in_use = false;
        for (int input = 0; input < NUM_INPUTS; input++)
        {
            bool this_trig_enabled;
            switch (trig)
            {
            case 0:
                this_trig_enabled = (*bounce_inputs[input]).get_param(BOUNCE_ENABLE_T0);
                break;
            case 1:
                this_trig_enabled = (*bounce_inputs[input]).get_param(BOUNCE_ENABLE_T1);
                break;
            case 2:
                this_trig_enabled = (*bounce_inputs[input]).get_param(BOUNCE_ENABLE_T2);
                break;
            case 3:
                this_trig_enabled = (*bounce_inputs[input]).get_param(BOUNCE_ENABLE_T3);
                break;
            }
            if (this_trig_enabled && ((*bounce_inputs[input]).triggered))
            {
                ui.terminal_debug((*bounce_inputs[input]).name + " triggered by: " + String(trig));
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
                reset_inputs(0);
                break;
            case 1:
                digitalWrite(T1_LED, LOW);
                reset_inputs(1);
                break;
            case 2:
                digitalWrite(T2_LED, LOW);
                reset_inputs(2);
                break;
            case 3:
                digitalWrite(T3_LED, LOW);
                reset_inputs(3);
                break;
            }
        }
    }
}

// called on interrupt
void trigger_bounce(byte trig_num, int input_num)
{
    // (*triggers[trig_num]).state = state;
    // input_num = bounce_input();
    (*bounce_inputs[input_num]).trigger(trig_num);
    bounce_triggered = (*bounce_inputs[input_num]).triggered;
    // bounce_triggered = selected_trigger->state == TRIGGER_ACTIVE ? true : false;

    // bounce_triggered = true;

    bounce_triggered_by[input_num] = trig_num;
    bounce_reading = adc_read(input_num);
    digitalWrite(trig_num, HIGH);
}

String check_bounce()
{
    if (bounce_triggered && millis() > bounce_next_time)
    {
        // ui.terminal_debug("enabled: " + String(bounce_debug) + " trig_num: " + String(trig2.trig_num));
        // float reading = IN_FS_VOLTS * bounce_reading / 1650;
        uint16_t the_bounce_input = bounce_input();
        uint16_t output_num = (*bounce_inputs[the_bounce_input]).get_param(BOUNCE_OUTPUT);
        float output_scale = float((*bounce_inputs[the_bounce_input]).get_param_w_offset(BOUNCE_SCALE)) / 100.0;
        float output_offset = float((*bounce_inputs[the_bounce_input]).get_param_w_offset(BOUNCE_OFFSET)) * 19.65; // 20.48
        int bounce_offset_correction = get_raw_output_offset_correction(output_num);
        float reading = scale_adc(bounce_reading);
        // uint16_t dac_val = (bounce_reading * output_scale + 1660) * 1.234;
        uint16_t dac_val = (bounce_reading * output_scale * 1.16) + DAC_MID + output_offset + bounce_offset_correction;
        ui.terminal_debug(" Reading: " + String(bounce_reading) + " DAC val: " + String(dac_val) + " Offset: " + String(output_offset) + " corr: " + bounce_offset_correction);
        system_message = " Reading: " + String(reading) + "V<br>Triggered by: T" + String(bounce_triggered_by[the_bounce_input]);
        send_data_to_USB(' ');
        dac_out(output_num, dac_val);
        bounce_next_time = millis() + selected_fxn->get_param(BOUNCE_SAMPLE_TIME);
        bounce_triggered = (*bounce_inputs[the_bounce_input]).triggered = false;
        if (selected_fxn->get_param(BOUNCE_REPEAT))
        {
            trigger_bounce(bounce_triggered_by[the_bounce_input], the_bounce_input);
        }
        return "  CV" + String(the_bounce_input) + " Reading: " + String(reading) + "V   Triggered by: T" + String(bounce_triggered_by[the_bounce_input]) + "  ";
    }
    return "";
}
