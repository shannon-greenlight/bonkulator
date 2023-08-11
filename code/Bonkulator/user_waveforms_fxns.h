#include <Greenface_gadget.h>
#include <EEPROM_Arr16.h>

// #define USER_WAVEFORMS_NAME 0
// #define USER_WAVEFORMS_INDEX 1
// #define USER_WAVEFORMS_VALUE 2
// #define USER_WAVEFORMS_RECORD 3
// #define USER_WAVEFORMS_START 4
// #define NUM_USER_WAVEFORMS_PARAMS 5

enum
{
    USER_WAVEFORMS_NAME,
    USER_WAVEFORMS_INDEX,
    USER_WAVEFORMS_VALUE,
    USER_WAVEFORMS_RECORD,
    USER_WAVEFORMS_START,
    USER_WAVEFORMS_INIT_WAVE,
    NUM_USER_WAVEFORMS_PARAMS
};

#define NUM_USER_WAVEFORMS 8

#define USER_WAVEFORMS_START_IMMEDIATE 0
#define USER_WAVEFORMS_START_TRIGGER 1
#define USER_WAVEFORMS_START_PLUS_CHANGE 2
#define USER_WAVEFORMS_START_MINUS_CHANGE 3

#define USER_WAVEFORMS_NAME_MAXLEN 10

#define INIT_WAVEFORMS "Sine,Haystack,Ramp,Pyramid,Line,Maytag"
enum
{
    INIT_WAVE_SINE,
    INIT_WAVE_HAYSTACK,
    INIT_WAVE_RAMP,
    INIT_WAVE_PYRAMID,
    INIT_WAVE_LINE,
    INIT_WAVE_MAYTAG,
    NUM_INIT_WAVEFORMS,
};

// #define NUM_INIT_WAVEFORMS 6

volatile uint8_t user_waveform_num;         // for recording
volatile uint8_t user_waveforms_record_inc; // msecs per sample
volatile uint16_t user_waveforms_record_index;

typedef enum _recording_state
{
    RECORDING_IDLE = 0,
    RECORDING_ACTIVE = 1,
    RECORDING_AWAIT_TRIGGER = 2,
    RECORDING_AWAIT_PLUS_CHANGE = 3,
    RECORDING_AWAIT_MINUS_CHANGE = 4,
    RECORDING_DONE = 5,
} recording_state;

volatile recording_state user_waveforms_recording_state = RECORDING_IDLE;
const int record_threshold = 20;

// volatile bool user_waveforms_recording = false;

uint16_t _user_waveforms_mins[] = {0, 0, 1, 1, 0, 0};
uint16_t _user_waveforms_maxs[] = {0, 127, DAC_FS, 1000, 3, NUM_INIT_WAVEFORMS - 1};
uint16_t _user_waveforms_init_vals[] = {0, 0, DAC_FS / 2, 8, 0, INIT_WAVE_LINE};
String user_waveforms_labels[] = {"Name: ", "Index: ", "Value: ", "Sample time(ms): ", "Record on: ", "Init Wave: "};
String user_waveforms_string_params[] = {"$~", "", "", "", "Immediate,Trigger,+Change,-Change", INIT_WAVEFORMS};
bool user_waveforms_param_active[] = {0, 0, 1, 1, 0};

// User Wave definitions
uint16_t _user_waveform0_params[NUM_USER_WAVEFORMS_PARAMS];
uint16_t *user_waveform0_stuff[] = {_user_waveform0_params, _user_waveforms_mins, _user_waveforms_maxs, _user_waveforms_init_vals};
Greenface_gadget user_waveform0("User 0", user_waveforms_labels, user_waveform0_stuff, sizeof(_user_waveform0_params) / sizeof(_user_waveform0_params[0]));
EEPROM_String user_waveform0_name(USER_WAVEFORMS_NAME_MAXLEN);
EEPROM_String user_waveform0_string_vars[] = {user_waveform0_name};

uint16_t _user_waveform1_params[NUM_USER_WAVEFORMS_PARAMS];
uint16_t *user_waveform1_stuff[] = {_user_waveform1_params, _user_waveforms_mins, _user_waveforms_maxs, _user_waveforms_init_vals};
Greenface_gadget user_waveform1("User 1", user_waveforms_labels, user_waveform1_stuff, sizeof(_user_waveform0_params) / sizeof(_user_waveform0_params[0]));
EEPROM_String user_waveform1_name(USER_WAVEFORMS_NAME_MAXLEN);
EEPROM_String user_waveform1_string_vars[] = {user_waveform1_name};

uint16_t _user_waveform2_params[NUM_USER_WAVEFORMS_PARAMS];
uint16_t *user_waveform2_stuff[] = {_user_waveform2_params, _user_waveforms_mins, _user_waveforms_maxs, _user_waveforms_init_vals};
Greenface_gadget user_waveform2("User 2", user_waveforms_labels, user_waveform2_stuff, sizeof(_user_waveform0_params) / sizeof(_user_waveform0_params[0]));
EEPROM_String user_waveform2_name(USER_WAVEFORMS_NAME_MAXLEN);
EEPROM_String user_waveform2_string_vars[] = {user_waveform2_name};

uint16_t _user_waveform3_params[NUM_USER_WAVEFORMS_PARAMS];
uint16_t *user_waveform3_stuff[] = {_user_waveform3_params, _user_waveforms_mins, _user_waveforms_maxs, _user_waveforms_init_vals};
Greenface_gadget user_waveform3("User 3", user_waveforms_labels, user_waveform3_stuff, sizeof(_user_waveform0_params) / sizeof(_user_waveform0_params[0]));
EEPROM_String user_waveform3_name(USER_WAVEFORMS_NAME_MAXLEN);
EEPROM_String user_waveform3_string_vars[] = {user_waveform3_name};

uint16_t _user_waveform4_params[NUM_USER_WAVEFORMS_PARAMS];
uint16_t *user_waveform4_stuff[] = {_user_waveform4_params, _user_waveforms_mins, _user_waveforms_maxs, _user_waveforms_init_vals};
Greenface_gadget user_waveform4("User 4", user_waveforms_labels, user_waveform4_stuff, sizeof(_user_waveform0_params) / sizeof(_user_waveform0_params[0]));
EEPROM_String user_waveform4_name(USER_WAVEFORMS_NAME_MAXLEN);
EEPROM_String user_waveform4_string_vars[] = {user_waveform4_name};

uint16_t _user_waveform5_params[NUM_USER_WAVEFORMS_PARAMS];
uint16_t *user_waveform5_stuff[] = {_user_waveform5_params, _user_waveforms_mins, _user_waveforms_maxs, _user_waveforms_init_vals};
Greenface_gadget user_waveform5("User 5", user_waveforms_labels, user_waveform5_stuff, sizeof(_user_waveform0_params) / sizeof(_user_waveform0_params[0]));
EEPROM_String user_waveform5_name(USER_WAVEFORMS_NAME_MAXLEN);
EEPROM_String user_waveform5_string_vars[] = {user_waveform5_name};

uint16_t _user_waveform6_params[NUM_USER_WAVEFORMS_PARAMS];
uint16_t *user_waveform6_stuff[] = {_user_waveform6_params, _user_waveforms_mins, _user_waveforms_maxs, _user_waveforms_init_vals};
Greenface_gadget user_waveform6("User 6", user_waveforms_labels, user_waveform6_stuff, sizeof(_user_waveform0_params) / sizeof(_user_waveform0_params[0]));
EEPROM_String user_waveform6_name(USER_WAVEFORMS_NAME_MAXLEN);
EEPROM_String user_waveform6_string_vars[] = {user_waveform6_name};

uint16_t _user_waveform7_params[NUM_USER_WAVEFORMS_PARAMS];
uint16_t *user_waveform7_stuff[] = {_user_waveform7_params, _user_waveforms_mins, _user_waveforms_maxs, _user_waveforms_init_vals};
Greenface_gadget user_waveform7("User 7", user_waveforms_labels, user_waveform7_stuff, sizeof(_user_waveform0_params) / sizeof(_user_waveform0_params[0]));
EEPROM_String user_waveform7_name(USER_WAVEFORMS_NAME_MAXLEN);
EEPROM_String user_waveform7_string_vars[] = {user_waveform7_name};

Greenface_gadget *user_waveforms[NUM_USER_WAVEFORMS] = {
    &user_waveform0,
    &user_waveform1,
    &user_waveform2,
    &user_waveform3,
    &user_waveform4,
    &user_waveform5,
    &user_waveform6,
    &user_waveform7,
};

EEPROM_String *user_waveforms_string_vars[NUM_USER_WAVEFORMS] = {
    user_waveform0_string_vars,
    user_waveform1_string_vars,
    user_waveform2_string_vars,
    user_waveform3_string_vars,
    user_waveform4_string_vars,
    user_waveform5_string_vars,
    user_waveform6_string_vars,
    user_waveform7_string_vars,
};

// 8 user waveforms
uint16_t _user_waveform0_data[WAVEFORM_PARTS];
uint16_t _user_waveform1_data[WAVEFORM_PARTS];
uint16_t _user_waveform2_data[WAVEFORM_PARTS];
uint16_t _user_waveform3_data[WAVEFORM_PARTS];
uint16_t _user_waveform4_data[WAVEFORM_PARTS];
uint16_t _user_waveform5_data[WAVEFORM_PARTS];
uint16_t _user_waveform6_data[WAVEFORM_PARTS];
uint16_t _user_waveform7_data[WAVEFORM_PARTS];

uint16_t *_user_waveforms_data[NUM_USER_WAVEFORMS] = {
    _user_waveform0_data,
    _user_waveform1_data,
    _user_waveform2_data,
    _user_waveform3_data,
    _user_waveform4_data,
    _user_waveform5_data,
    _user_waveform6_data,
    _user_waveform7_data,
};

EEPROM_Arr16 user_waveform0_data(_user_waveform0_data, WAVEFORM_PARTS);
EEPROM_Arr16 user_waveform1_data(_user_waveform1_data, WAVEFORM_PARTS);
EEPROM_Arr16 user_waveform2_data(_user_waveform2_data, WAVEFORM_PARTS);
EEPROM_Arr16 user_waveform3_data(_user_waveform3_data, WAVEFORM_PARTS);
EEPROM_Arr16 user_waveform4_data(_user_waveform4_data, WAVEFORM_PARTS);
EEPROM_Arr16 user_waveform5_data(_user_waveform5_data, WAVEFORM_PARTS);
EEPROM_Arr16 user_waveform6_data(_user_waveform6_data, WAVEFORM_PARTS);
EEPROM_Arr16 user_waveform7_data(_user_waveform7_data, WAVEFORM_PARTS);

EEPROM_Arr16 *user_waveforms_data[NUM_USER_WAVEFORMS] = {
    &user_waveform0_data,
    &user_waveform1_data,
    &user_waveform2_data,
    &user_waveform3_data,
    &user_waveform4_data,
    &user_waveform5_data,
    &user_waveform6_data,
    &user_waveform7_data,
};

void user_waveforms_init()
{
    for (int i = 0; i < NUM_USER_WAVEFORMS; i++)
    {
        // user_waveform->string_vars[0].put("User Waverform " + String(i));
        (*user_waveforms[i]).init();
        // (*user_waveforms[i]).param_put(INIT_WAVE_LINE, USER_WAVEFORMS_INIT_WAVE);
        (*user_waveforms[i]).put_string_var("User " + String(i), USER_WAVEFORMS_NAME);
        (*user_waveforms_data[i]).fill(DAC_MID);
    }
}

bool in_user_waveform_fxn()
{
    // return true if we are in a user_waveform fxn
    for (int i = 0; i < NUM_USER_WAVEFORMS; i++)
    {
        if (selected_fxn == user_waveforms[i])
            return true;
    }
    return false;
}

int get_user_waveform_num()
{
    for (int i = 0; i < NUM_USER_WAVEFORMS; i++)
    {
        if (selected_fxn == user_waveforms[i])
            return i;
    }
    return -1;
}

void graph_user_waveform(int waveform_num)
{
    // ui.terminal_debug("graph user waveform: " + String(waveform_num));
    // ui.graph_waveform(_user_waveforms_data[waveform_num], user_waveforms[waveform_num]->get_param(USER_WAVEFORMS_PARTS));
    ui.graph_waveform(_user_waveforms_data[waveform_num], WAVEFORM_PARTS, DAC_FS);
    ui.drawXHash(user_waveforms[waveform_num]->get_param(USER_WAVEFORMS_INDEX), ui.fit_data(user_waveforms[waveform_num]->get_param(USER_WAVEFORMS_VALUE), DAC_FS));
}

void user_waveforms_update_waveform()
{
    graph_user_waveform(get_user_waveform_num());
}

void user_waveforms_update_index()
{
    int waveform_num = get_user_waveform_num();
    user_waveforms[waveform_num]->put_param((*user_waveforms_data[waveform_num]).get(user_waveforms[waveform_num]->get_param(USER_WAVEFORMS_INDEX)), USER_WAVEFORMS_VALUE);
    graph_user_waveform(get_user_waveform_num());
}

void user_waveforms_put_val(int waveform_num, uint16_t val, int index)
{
    (*user_waveforms_data[waveform_num]).put(val, index);
}

void user_waveforms_update_value()
{
    int waveform_num = get_user_waveform_num();
    user_waveforms_put_val(waveform_num, user_waveforms[waveform_num]->get_param(USER_WAVEFORMS_VALUE), user_waveforms[waveform_num]->get_param(USER_WAVEFORMS_INDEX));
    graph_user_waveform(waveform_num);
}

void user_waveforms_update_name()
{
    String names;
    for (int i = 0; i < NUM_USER_WAVEFORMS; i++)
    {
        Greenface_gadget *user_waveform = user_waveforms[i];
        names += ",";
        names += user_waveform->get_param_as_string_var(USER_WAVEFORMS_NAME);
    }
    update_user_waveform_names(names);
}

void user_waveforms_update_start()
{
    user_waveforms_recording_state = RECORDING_IDLE;
}

void user_waveforms_update_init_wave()
{
    int waveform_num = get_user_waveform_num();
    int init_wave_num = user_waveforms[waveform_num]->get_param(USER_WAVEFORMS_INIT_WAVE);
    // ui.terminal_debug("Init wave: " + String(init_wave_num));
    for (int i = 0; i < WAVEFORM_PARTS; i++)
    {
        if (init_wave_num == INIT_WAVE_LINE)
        {
            user_waveforms_put_val(waveform_num, DAC_MID, i);
        }
        else
        {
            user_waveforms_put_val(waveform_num, calc_wave_value(i, init_wave_num, WAVEFORM_PARTS), i);
        }
    }
}

update_fxn user_waveforms_update_fxns[NUM_USER_WAVEFORMS_PARAMS] = {user_waveforms_update_name, user_waveforms_update_index, user_waveforms_update_value, nullptr, user_waveforms_update_start, user_waveforms_update_init_wave};

void user_waveforms_begin()
{
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        Greenface_gadget *user_waveform = user_waveforms[i];
        user_waveform->begin();
        user_waveform->string_params = user_waveforms_string_params;
        user_waveform->update_fxns = user_waveforms_update_fxns;
        user_waveform->active_params = user_waveforms_param_active;

        user_waveform->string_vars = user_waveforms_string_vars[i];
        user_waveform->string_vars[0].begin(false);
        user_waveform->string_vars[0].xfer();

        (*user_waveforms_data[i]).begin(false);
        (*user_waveforms_data[i]).xfer();
    }
    user_waveforms_record_inc = 0;
    user_waveforms_update_name();
    // outputs_begin(); // do it again to init user waveforms properly
}

void timer_service_user_waveforms_record()
{
    if (Timer0Count % user_waveforms_record_inc == 0)
    {
        float adc_val = (float)adc0;
        adc_val *= -1;
        adc_val += 1650.0;
        adc_val = DAC_FS * (adc_val / 3300.0);

        adc_val = constrain(adc_val, 0, DAC_FS);
        int16_t val = (int)adc_val;

        uint16_t *user_waveform_data = _user_waveforms_data[user_waveform_num];
        user_waveform_data[user_waveforms_record_index] = val;

        // inc pointer and test for done
        user_waveforms_record_index++;
        if (user_waveforms_record_index == WAVEFORM_PARTS)
        {
            user_waveforms_recording_state = RECORDING_DONE;
            timer_fxn = &timer_service_settings;
        }
    }
}

void user_waveforms_start_recording()
{
    // ui.terminal_debug("Record user waveform: " + String(user_waveform_num));
    user_waveform_num = get_user_waveform_num();
    user_waveforms_recording_state = RECORDING_ACTIVE;
    user_waveforms_record_index = Timer0Count = 0;
    user_waveforms_record_inc = selected_fxn->get_param(USER_WAVEFORMS_RECORD);
    timer_fxn = &timer_service_user_waveforms_record;
}

void user_waveforms_activate()
{
    switch (selected_fxn->param_num)
    {
    case USER_WAVEFORMS_VALUE:
        // increment index
        selected_fxn->put_param(selected_fxn->get_param(USER_WAVEFORMS_INDEX) + 1, USER_WAVEFORMS_INDEX);
        // store value
        selected_fxn->put_param(selected_fxn->get_param(USER_WAVEFORMS_VALUE), USER_WAVEFORMS_VALUE);
        break;
    case USER_WAVEFORMS_RECORD:
        switch (selected_fxn->get_param(USER_WAVEFORMS_START))
        {
        case USER_WAVEFORMS_START_IMMEDIATE:
            user_waveforms_start_recording();
            break;
        case USER_WAVEFORMS_START_TRIGGER:
            user_waveforms_recording_state = RECORDING_AWAIT_TRIGGER;
            break;
        case USER_WAVEFORMS_START_PLUS_CHANGE:
            user_waveforms_recording_state = RECORDING_AWAIT_PLUS_CHANGE;
            break;
        case USER_WAVEFORMS_START_MINUS_CHANGE:
            user_waveforms_recording_state = RECORDING_AWAIT_MINUS_CHANGE;
            break;
        }
        break;
    default:
        break;
    }
}

String check_user_recording()
{
    static int idle_reading;
    if (selected_fxn->param_num == USER_WAVEFORMS_RECORD)
    {
        user_waveforms_record_inc = selected_fxn->get_param(USER_WAVEFORMS_RECORD);
        switch (user_waveforms_recording_state)
        {
        case RECORDING_IDLE:
            idle_reading = ads.readADC_Differential_0_3();
            return String(ACTIVATE_STRING) + " to start recording: " + String(selected_fxn->get_param_as_string_var(USER_WAVEFORMS_NAME)) + " every " + String(user_waveforms_record_inc) + " msecs.";
            break;
        case RECORDING_AWAIT_TRIGGER:
            if (check_any_triggers())
            {
                user_waveforms_start_recording();
            }
            return "Waiting for Trigger... ";
            break;
        case RECORDING_AWAIT_PLUS_CHANGE:
            if (idle_reading - ads.readADC_Differential_0_3() > record_threshold)
            {
                user_waveforms_start_recording();
            }
            return "Waiting for Plus change... ";
            break;
        case RECORDING_AWAIT_MINUS_CHANGE:
            if (ads.readADC_Differential_0_3() - idle_reading > record_threshold)
            {
                user_waveforms_start_recording();
            }
            return "Waiting for Minus change... ";
            break;
        case RECORDING_ACTIVE:
            adc0 = ads.readADC_Differential_0_3();
            return "Recording: " + String(selected_fxn->get_param_as_string_var(USER_WAVEFORMS_NAME)) + " every " + String(user_waveforms_record_inc) + " msecs.";
            break;
        case RECORDING_DONE:
            // save data here!
            if (user_waveforms_record_index == WAVEFORM_PARTS)
            {
                for (int i = 0; i < WAVEFORM_PARTS; i++)
                {
                    // Serial.println("i: " + String(i) + " val: " + String((*user_waveforms_data[user_waveform_num]).get(i)));
                    (*user_waveforms_data[user_waveform_num]).put((*user_waveforms_data[user_waveform_num]).get(i), i);
                }
                user_waveforms_record_index = 0;
            }
            idle_reading = ads.readADC_Differential_0_3();
            return "Done recording";
            break;
        default:
            return "";
        }
    }
    else
    {
        return "";
    }
}

void user_waveforms_debug()
{
}
