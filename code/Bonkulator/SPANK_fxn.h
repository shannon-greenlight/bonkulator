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
#include <EEPROM_Int.h>
#include <EEPROM_Arr16.h>
#include <EEPROM_String.h>
#include "GREENFACE_ui.h"

#define SPANK_INT_PARAM_TYPE 0
#define SPANK_STRING_PARAM_TYPE 1
#define SPANK_STRING_VAR_TYPE 2
#define SPANK_FLOAT_VAR_TYPE 3

extern String toJSON(String, String);

typedef void (*update_fxn)(void);

// library interface description
class SPANK_fxn
{
public:
    typedef void (SPANK_fxn::*FunctionPointer)();
    SPANK_fxn(String _name, String *_labels, uint16_t **_params, uint16_t _num_params, GREENFACE_ui *_ui);
    String name;
    String *labels;
    String *string_params;
    String *alt_values;
    EEPROM_String *string_vars;
    int16_t *offsets;
    bool *active_params;
    boolean check_params = false;

    uint16_t num_params;
    int param_num;
    uint8_t digit_num;
    void begin(void);
    void init();
    uint8_t get_param_type(int);
    uint16_t get_param(int16_t indx = -1);     // returns raw
    int get_param_w_offset(int16_t indx = -1); // returns offset param as int
    String get_param_as_string(int16_t indx = -1);
    String get_param_as_string_var(int16_t indx = -1);
    bool get_param_active(int16_t indx = -1);
    void put_string_var(String val, int16_t indx = -1);
    uint16_t get_min(int16_t indx = -1);
    uint16_t get_max(int16_t indx = -1);
    int get_min_w_offset(int16_t indx = -1);
    int get_max_w_offset(int16_t indx = -1);
    void adjust_param(int e, unsigned long delta);
    void param_put(uint16_t val, int8_t _param_num = -1); // raw no testing, printing
    void put_param(uint16_t val, int8_t _param_num = -1);
    void put_param_w_offset(int val, int8_t _param_num = -1);
    void insert_char(char);
    void remove_char();
    void put_param_num(int val);
    int check_param(int the_param);
    void inc_param_num_by(int val);
    void put_dig_num(int val);
    void inc_dig_num_by(int val);
    void hilight_param();
    String get_label(int label_num);
    void printParam(int8_t _param_num = -1);
    void printParams();
    String params_toJSON(); // for wifi

    void trigger(uint8_t);
    void clear_trigger(uint8_t);
    void display(void); // calls vectored fxn
    void copy_to(SPANK_fxn &target);
    void debug(void);
    uint16_t *mins;
    uint16_t *maxs;
    uint16_t *init_vals;

    void default_display(void); // prints name and params
    byte display_offset = 0;    // user fxn sets this to 1 to make room for sequence
    boolean enable_hilight = true;

    int offset(void);
    uint8_t triggered_by;

    // vectored fxns
    void (*display_fxn)();
    update_fxn *update_fxns;

    bool triggered;
    void (*trigger_fxn)();

    // library-accessible "private" interface
private:
    GREENFACE_ui *ui;
    EEPROM_Arr16 params;
    uint8_t get_num_digits(int);
    String calc_format(uint8_t);
    void print_param(uint16_t, uint16_t);
    void print_string_param(uint16_t, uint16_t);
    void print_string_var(uint16_t, uint16_t);
    void exe_update_fxn(void);
    // const uint8_t int_param_type = 0;
    // const uint8_t string_param_type = 1;
    // const uint8_t string_var_type = 2;
};
#endif
