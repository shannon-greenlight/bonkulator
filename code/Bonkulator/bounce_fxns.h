#include <Wire.h>

enum
{
    BOUNCE_OUTPUT,
    BOUNCE_REPEAT,
    BOUNCE_ENABLE_T0,
    BOUNCE_ENABLE_T1,
    BOUNCE_ENABLE_T2,
    BOUNCE_ENABLE_T3,
    BOUNCE_CV0,
    BOUNCE_CV1,
    BOUNCE_SCALE,
    BOUNCE_OFFSET,
    BOUNCE_SAMPLE_TIME,
    BOUNCE_TRIG_CTRL,
    BOUNCE_TRIG_CTRL_VAL,
    BOUNCE_NUM_PARAMS,
};

uint16_t _bounce_mins[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, TRIG_HOLDOFF, 0};
uint16_t _bounce_maxs[] = {7, 1, 1, 1, 1, 1, CV_TRIGGER, CV_TRIGGER, 100, 100, 32767, TRIG_DENSITY, MAX_MS};
uint16_t _bounce_init_vals[] = {0, 0, 0, 0, 0, 0, 0, 0, 100, 0, 100, TRIG_HOLDOFF, 5};
String bounce_string_params[] = {"", "No, Yes", TRIGGER_STATES, TRIGGER_STATES, TRIGGER_STATES, TRIGGER_STATES, CV_TYPES, CV_TYPES, "", "", "", "Hold_Off,Skip,Density", ""};
// int16_t bounce_offsets[] = {0, BOUNCE_OFFSET_CORRECTION, 0}; // allows negative numbers

uint16_t _bounce0_params[BOUNCE_NUM_PARAMS];
uint16_t *bounce0_stuff[] = {_bounce0_params, _bounce_mins, _bounce_maxs, _bounce_init_vals};
String bounce0_labels[] = {"Output: ", "Repeat: ", "T0: ", "T1: ", "T2: ", "T3: ", "CV0: ", "CV1: ", "Scale: ", "Offset: ", "Sample Time: ", "Trig Crtl: ", "Trig Holdoff: "};
Greenface_gadget bounce0_fxn("Bounce 0", bounce0_labels, bounce0_stuff, sizeof(_bounce0_params) / sizeof(_bounce0_params[0]));

uint16_t _bounce1_params[BOUNCE_NUM_PARAMS];
uint16_t *bounce1_stuff[] = {_bounce1_params, _bounce_mins, _bounce_maxs, _bounce_init_vals};
String bounce1_labels[] = {"Output: ", "Repeat: ", "T0: ", "T1: ", "T2: ", "T3: ", "CV0: ", "CV1: ", "Scale: ", "Offset: ", "Sample Time: ", "Trig Crtl: ", "Trig Holdoff: "};
Greenface_gadget bounce1_fxn("Bounce 1", bounce1_labels, bounce1_stuff, sizeof(_bounce1_params) / sizeof(_bounce1_params[0]));

Greenface_gadget *bounce_inputs[NUM_OUTPUTS] = {
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
    for (int i = 0; i < 2; i++)
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

// called on interrupt
void trigger_bounce(byte trig_num, trigger_state state)
{
    bounce_triggered = true;
    bounce_triggered_by[bounce_input()] = trig_num;
    bounce_reading = adc_read(bounce_input());
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

update_fxn bounce_update_fxns[BOUNCE_NUM_PARAMS] = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
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
