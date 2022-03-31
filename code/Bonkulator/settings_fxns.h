#include <Wire.h>

#define SETTINGS_VER 0
#define SETTINGS_NAME 1
#define SETTINGS_ENCODER 2
#define SETTINGS_WIFI 3
#define SETTINGS_SCREEN 4
#define SETTINGS_RESET 5
#define SETTINGS_CAL 6

#define HEART_RATE 25 // todo adapt this to new OS

uint16_t _settings_params[7];
uint16_t _settings_mins[] = {0, 0, 0, 0, 1, 0, 0};
uint16_t _settings_maxs[] = {0, 0, 1, 1, 9999, 1, 1};
uint16_t _settings_init_vals[] = {0, 0, 1, 1, 15, 0, 0};
uint16_t *settings_stuff[] = {_settings_params, _settings_mins, _settings_maxs, _settings_init_vals};
String settings_labels[] = {"Version: ", "Name: ", "Encoder Type: ", "WiFi: ", "Screen Saver: ", "Reset: ", "Calibrate: "};
String settings_string_params[] = {VERSION_NUM, "$~", "Normal ,Reverse", "Disabled,Enabled ", "", "Push Sel", "ADC,DAC"};
EEPROM_String settings_device_name(20);
EEPROM_String settings_string_vars[] = {settings_device_name};
SPANK_fxn settings_fxn("Settings", settings_labels, settings_stuff, sizeof(_settings_params) / sizeof(_settings_params[0]), &ui);

void settings_display()
{
    // adc_cal_screen = 0;
    // dac_cal_screen = 0;
    settings_fxn.display();
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
        if (in_wifi)
        {
            wifi_adjust_param(encoder_val);
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

// void settings_trigger()
// {
//     switch (settings_fxn.param_num)
//     {
//     case SETTINGS_RESET:
//         init_all();
//         break;

//     case SETTINGS_CAL:
//         switch (settings_fxn.get_param(SETTINGS_CAL))
//         {
//         case 0:
//             adc_cal();
//             break;
//         case 1:
//             dac_cal();
//             break;
//         }
//         break;

//     default:
//         ui.terminal_debug("Initialized: " + String(is_initialized()) + " Pattern: " + String(EEPROM_INIT_PATTERN));
//         break;
//     }
// }

void settings_init()
{
    settings_fxn.string_vars[0].put("Bonky");
}

void settings_begin()
{
    settings_fxn.begin();
    settings_fxn.string_vars = settings_string_vars;
    settings_fxn.string_vars[0].begin(false);
    settings_fxn.string_vars[0].xfer();
    settings_fxn.string_params = settings_string_params;
}

// void settings_begin(void);
// void settings_init(void);
// void settings_print_param(uint8_t);
// void settings_print_params(void);
// uint8_t settings_param_num;
// uint8_t settings_num_params;
// uint8_t settings_line_num;
// void settings_inc_param_num();
// void settings_dec_param_num();
// uint8_t settings_digit_num;
// void settings_inc_digit_num();
// void settings_dec_digit_num();
// void settings_display(void);

// #define SETTINGS_VER_FXN 0
// #define SETTINGS_NAME_FXN 1
// #define SETTINGS_ENC_TYPE_FXN 2
// #define SETTINGS_WIFI_ENABLE_FXN 3
// #define SETTINGS_RESET_FXN 4
// #define SETTINGS_CALIBRATE_FXN 5
// #define SETTINGS_NUM_PARAMS 6

// // access fxns
// String settings_get_device_name()
// {
//     return "Bonky";
// }

// int settings_get_encoder_type()
// {
//     return 0;
// }

// String settings_get_wifi_enable()
// {
//     return "Enabled";
// }

// String settings_get_calibrate_type()
// {
//     return "Cal DC";
// }

// // display fxns
// void settings_ver()
// {
//     ui.printLine("Software ver: " + String(VERSION_NUM), ui.lines[settings_line_num], 1);
//     // ui.printParam("Software ver: ", time_inc.get(), ndigs, format, settings_line_num);
// }

// void settings_name()
// {
//     ui.printLine("Name: " + settings_get_device_name(), ui.lines[settings_line_num], 1);
//     // ui.printParam("Software ver: ", time_inc.get(), ndigs, format, settings_line_num);
// }

// void settings_enc_type()
// {
//     ui.printLine("Encoder type: " + settings_get_encoder_type() ? "Normal" : "Reverse", ui.lines[settings_line_num], 1);
//     // ui.printParam("Software ver: ", time_inc.get(), ndigs, format, settings_line_num);
// }

// void settings_wifi_enable()
// {
//     ui.printLine("WiFi: " + settings_get_wifi_enable(), ui.lines[settings_line_num], 1);
//     // ui.printParam("Software ver: ", time_inc.get(), ndigs, format, settings_line_num);
// }

// void settings_reset()
// {
//     ui.printLine("Reset: Push for Factory Defaults", ui.lines[settings_line_num], 1);
//     // ui.printParam("Software ver: ", time_inc.get(), ndigs, format, settings_line_num);
// }

// void settings_calibrate()
// {
//     ui.printLine("Calibrate: " + settings_get_calibrate_type(), ui.lines[settings_line_num], 1);
//     // ui.printParam("Software ver: ", time_inc.get(), ndigs, format, settings_line_num);
// }

// FunctionPointer settings_fxns[] = {settings_ver, settings_name, settings_enc_type, settings_wifi_enable, settings_reset, settings_calibrate};

// void settings_print_params()
// {
//     for (int i = 0; i < 3; i++)
//     {
//         settings_line_num = i;
//         (*settings_fxns[i])();
//     }
// }

// void settings_display()
// {
//     ui.clearDisplay();
//     ui.printLine("Settings ", LINE_0);
//     settings_print_params();
// }

// void settings_print_params()
// {
//     Serial.println("Settings params!");
// }
