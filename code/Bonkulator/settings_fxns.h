#include <Wire.h>

#define SETTINGS_VER 0
#define SETTINGS_NAME 1
#define SETTINGS_ENCODER 2
#define SETTINGS_WIFI 3
#define SETTINGS_SCREEN 4
#define SETTINGS_USER_WAVEFORMS 5
#define SETTINGS_RESET 6
#define SETTINGS_CAL 7
#define SETTINGS_NUM_PARAMS 8

uint16_t _settings_params[SETTINGS_NUM_PARAMS];
uint16_t _settings_mins[] = {0, 0, 0, 0, 1, 0, 0, 0};
uint16_t _settings_maxs[] = {0, 0, 1, 1, 9999, 7, 1, 1};
uint16_t _settings_init_vals[] = {0, 0, 1, 0, 15, 0, 0, 0};
uint16_t *settings_stuff[] = {_settings_params, _settings_mins, _settings_maxs, _settings_init_vals};
String settings_labels[] = {"Version: ", "Name: ", "Encoder Type: ", "WiFi: ", "Screen Saver: ", "Waveforms: ", "Reset: ", "Calibrate: "};
String settings_string_params[] = {VERSION_NUM, "$~", "Normal ,Reverse", "Disabled,Enabled ", "", "Wave 0,Wave 1,Wave 2,Wave 3,Wave 4,Wave 5,Wave 6,Wave 7", ACTIVATE_STRING, "Inputs,Outputs"};
bool settings_param_active[] = {0, 0, 0, 1, 0, 1, 1, 1};

// void (*func_ptr[3])() = {fun1, fun2, fun3};

update_fxn settings_update_fxns[SETTINGS_NUM_PARAMS] = {nullptr, nullptr, set_encoder, nullptr, nullptr, nullptr, nullptr, nullptr};

EEPROM_String settings_device_name(20);
EEPROM_String settings_string_vars[] = {settings_device_name};
Greenface_gadget settings_fxn("Settings", settings_labels, settings_stuff, sizeof(_settings_params) / sizeof(_settings_params[0]));

#define ASK_INIT_PARAMETERS 0
#define ASK_INIT_INPUTS 1
#define ASK_INIT_OUTPUTS 2
#define ASK_INIT_ALL 3

uint16_t _ask_init_params[1];
uint16_t _ask_init_mins[] = {0};
uint16_t _ask_init_maxs[] = {3};
uint16_t _ask_init_init_vals[] = {0};
uint16_t *ask_init_stuff[] = {_ask_init_params, _ask_init_mins, _ask_init_maxs, _ask_init_init_vals};
String ask_init_labels[] = {"Reset: "};
String ask_init_string_params[] = {"Parameters  ,Input Corrs ,Output Corrs,All         "};
bool ask_init_param_active[] = {1};

Greenface_gadget ask_init_fxn("Press Activate to: ", ask_init_labels, ask_init_stuff, sizeof(_ask_init_params) / sizeof(_ask_init_params[0]));

bool in_ask_init()
{
    return selected_fxn == &ask_init_fxn;
}

void ask_init()
{
    int init_type = ask_init_fxn.get_param(0);
    switch (init_type)
    {
    case ASK_INIT_PARAMETERS:
        init_parameters();
        break;
    case ASK_INIT_INPUTS:
        input_corrections_init();
        break;
    case ASK_INIT_OUTPUTS:
        output_corrections_init();
        break;
    case ASK_INIT_ALL:
        init_all();
        break;
    }
    status_string = ask_init_fxn.get_param_as_string(0) + " Reset";
    selected_fxn->display();
    terminal_print_status(true);
}

void settings_put_param(int val)
{
    settings_fxn.put_param(val);
    switch (settings_fxn.param_num)
    {
    case SETTINGS_ENCODER:
        set_encoder();
        break;
    case SETTINGS_SCREEN:
        restore_display();
        break;
    }
}

void settings_adjust_param(int encoder_val, unsigned long delta)
{
    switch (settings_fxn.param_num)
    {
    case SETTINGS_ENCODER:
        settings_fxn.adjust_param(encoder_val, delta);
        set_encoder();
        break;
    case SETTINGS_SCREEN:
        settings_fxn.adjust_param(encoder_val, delta);
        restore_display();
        break;
    default:
        settings_fxn.adjust_param(encoder_val, delta);
    }
}

String settings_get_device_name()
{
    return settings_fxn.get_param_as_string_var(SETTINGS_NAME);
}

uint16_t settings_get_encoder_type()
{
    return settings_fxn.get_param(SETTINGS_ENCODER);
}

bool wifi_enabled()
{
    return settings_fxn.get_param(SETTINGS_WIFI);
}

int settings_get_inactivity_timeout()
{
    return settings_fxn.get_param(SETTINGS_SCREEN);
    // return max(300, settings_fxn.get_param(SETTINGS_SCREEN) * 300); // 1 minute = 300 heartbeats
}

void adc_cal()
{
    ui.terminal_debug("Calibrate ADC someday...");
}

void dac_cal()
{
    // ui.terminal_debug("Calibrate DAC someday...");
    // for (int i = 0; i < NUM_OUTPUTS; i++)
    // {
    //     (*bonk_outputs[i]).put_param(512, OUTPUT_IDLE_VALUE);
    //     // dac_out(i, 512);
    // }
}

void settings_activate()
{
    switch (settings_fxn.param_num)
    {
    case SETTINGS_WIFI:
        selected_fxn = &wifi_fxn;
        // selected_fxn->display();
        wifi_display();
        // wifi_activate();
        break;
    case SETTINGS_USER_WAVEFORMS:
        // ui.terminal_debug("User Waveforms!");
        selected_fxn = user_waveforms[settings_fxn.get_param(SETTINGS_USER_WAVEFORMS)];
        user_waveforms_update_index();
        user_waveforms_recording_state = RECORDING_IDLE;
        selected_fxn->display();
        // ui.terminal_debug("User Waveforms: setting param# " + String(settings_fxn.param_num));
        break;

    case SETTINGS_RESET:
        selected_fxn = &ask_init_fxn;
        selected_fxn->display();
        break;

    case SETTINGS_CAL:
        switch (settings_fxn.get_param(SETTINGS_CAL))
        {
        case 0:
            selected_fxn = &input_cal_fxn;
            selected_fxn->display();
            // adc_cal();
            break;
        case 1:
            selected_fxn = &output_cal_fxn;
            selected_fxn->display();
            dac_cal();
            break;
        }
        break;

    default:
        // ui.terminal_debug("Initialized: " + String(is_initialized()) + " Pattern: " + String(EEPROM_INIT_PATTERN));
        break;
    }
}

void settings_init()
{
    settings_fxn.string_vars[0].put("Bonky");
    settings_fxn.init();
    ask_init_fxn.init();
}

void settings_begin()
{
    settings_fxn.begin();
    settings_fxn.string_vars = settings_string_vars;
    settings_fxn.string_vars[0].begin(false);
    settings_fxn.string_vars[0].xfer();
    settings_fxn.string_params = settings_string_params;
    settings_fxn.update_fxns = settings_update_fxns;
    settings_fxn.active_params = settings_param_active;
    ask_init_fxn.begin();
    ask_init_fxn.string_params = ask_init_string_params;
}

void timer_service_settings()
{
    // do nothing for now
}

void user_waveform_params_macro(int waveform_num, bool print_header = true)
{
    Greenface_gadget *fxn = user_waveforms[waveform_num];
    uint16_t *waveform_data = _user_waveforms_data[waveform_num];
    // fxn->param_num = USER_WAVEFORMS_VALUE;
    if (print_header)
    {
        ui.t.clrScreen();
        ui.terminal_debug("// Macro for: " + fxn->name);
    }
    Serial.println("*p" + String(SETTINGS_USER_WAVEFORMS));
    Serial.println(String(waveform_num));
    Serial.println("!");
    gen_params_macro(fxn, false);

    Serial.println("p" + String(USER_WAVEFORMS_INDEX));
    Serial.println("0");
    Serial.println("p" + String(USER_WAVEFORMS_VALUE));

    for (int i = 0; i < WAVEFORM_PARTS; i++)
    {
        Serial.println(String(waveform_data[i]));
        Serial.println("!");
    }
}
