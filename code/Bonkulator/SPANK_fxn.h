// ensure this library description is only included once
#ifndef SPANK_fxn_h
#define SPANK_fxn_h

#define LONGEST_PULSE 0
#define SHORTEST_PULSE 1
#define NUM_PULSES 2
#define RANDOMNESS 3
#define INITIAL_DELAY 4

// include types & constants of Wiring core API
#include "Arduino.h"
#include <TerminalVT100.h>
#include <Greenface_EEPROM.h>
#include <EEPROM_Arr16.h>
#include <EEPROM_String.h>
#include "GREENFACE_ui.h"

#define SPANK_INT_PARAM_TYPE 0
#define SPANK_STRING_PARAM_TYPE 1
#define SPANK_STRING_VAR_TYPE 2
extern String toJSON(String, String);
// extern String settings_get_device_name();

// library interface description
class SPANK_fxn
{
    // user-accessible "public" interface
public:
    typedef void (SPANK_fxn::*FunctionPointer)();
    SPANK_fxn(String _name, String *_labels, uint16_t **_params, uint16_t _num_params, GREENFACE_ui *_ui);
    String name;
    String *labels;
    String *string_params;
    EEPROM_String *string_vars;
    boolean check_params = false;

    uint16_t num_params;
    int param_num;
    uint8_t digit_num;
    void begin(void);
    void init();
    uint16_t get_param(int16_t indx = -1);
    String get_param_as_string(int16_t indx = -1);
    String get_param_as_string_var(int16_t indx = -1);
    void put_string_var(String val, int16_t indx = -1);
    uint16_t get_min(int16_t indx = -1);
    uint16_t get_max(int16_t indx = -1);
    void adjust_param(int e, unsigned long delta);
    void put_param(uint16_t val);
    void put_param_num(int val);
    int check_param(int the_param);
    void inc_param_num_by(int val);
    void put_dig_num(int val);
    void inc_dig_num_by(int val);
    void hilight_param();
    String get_label(int label_num);
    void printParam();
    void printParams();
    String params_toJSON(); // for wifi

    void display(void); // calls vectored fxn
    void copy_to(SPANK_fxn &target);
    void debug(void);
    uint16_t *mins;
    uint16_t *maxs;
    uint16_t *init_vals;

    byte display_offset = 0; // user sets this to 1 to make room for sequence
    boolean enable_hilight = true;

    int offset(void);

    // vectored fxns
    FunctionPointer display_fxn;
    void (*trigger_fxn)();

    // library-accessible "private" interface
private:
    GREENFACE_ui *ui;
    EEPROM_Arr16 params;
    void default_display(void); // prints name and params
    uint8_t get_num_digits(int);
    String calc_format(uint8_t);
    void print_param(uint16_t, uint16_t);
    void print_string_param(uint16_t, uint16_t);
    void print_string_var(uint16_t, uint16_t);
    uint8_t get_param_type(int);
    // const uint8_t int_param_type = 0;
    // const uint8_t string_param_type = 1;
    // const uint8_t string_var_type = 2;
};
#endif
