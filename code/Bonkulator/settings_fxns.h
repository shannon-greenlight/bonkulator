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

#define HEART_RATE 25 // todo adapt this to new OS

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
SPANK_fxn settings_fxn("Settings", settings_labels, settings_stuff, sizeof(_settings_params) / sizeof(_settings_params[0]), &ui);

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
        if (false && in_wifi)
        {
            // wifi_adjust_param(encoder_val);
        }
        else
        {
            settings_fxn.adjust_param(encoder_val, delta);
        }
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
    return settings_fxn.get_param(SETTINGS_SCREEN) * HEART_RATE * 60;
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
    //     (*bonk_outputs[i]).put_param(512, OUTPUT_DACVAL);
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
        selected_fxn->display();
        // ui.terminal_debug("User Waveforms: setting param# " + String(settings_fxn.param_num));
        break;

    case SETTINGS_RESET:
        init_all();
        break;

    case SETTINGS_CAL:
        switch (settings_fxn.get_param(SETTINGS_CAL))
        {
        case 0:
            adc_cal();
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
}

void timer_service_settings()
{
    // do nothing for now
}